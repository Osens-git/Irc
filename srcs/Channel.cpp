/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 11:16:16 by cgelgon           #+#    #+#             */
/*   Updated: 2025/11/30 18:31:32 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(){}
Channel::Channel(const Channel&){}
Channel::~Channel() {}

Channel::Channel(const std::string& name) : _name(name), _topic(""), _key(""),
	_invite_only(false), _topic_restricted(false), _user_limit(-1) {}

//Getters
std::string	Channel::getName()				const { return _name; }
std::string Channel::getTopic()				const { return _topic; }
std::string	Channel::getKey()				const { return _key; }
bool		Channel::isInviteOnly()			const { return _invite_only; }
bool		Channel::isTopicRestricted()	const { return _topic_restricted; }

//Setters
void Channel::setTopic(std::string topic)			{ _topic = topic; }
void Channel::setKey(std::string key)				{ _key = key; }
void Channel::setInviteOnly(bool value)				{ _invite_only = value; }
void Channel::setTopicRestricted(bool value)		{ _topic_restricted = value; }
void Channel::setUserLimit(size_t limit)			{ _user_limit = limit; }

// Methodes
int Channel::addMember(Client *cli, std::string key)
{
	if (this->ismember(cli))
		return (1);

	if (_user_limit > 0 && _members.size() >= _user_limit)
	{
		std::string msg = return_cmd_failure(471, _name + " ", "Cannot join channel (+l full)");
		send(cli->get_fd(), msg.c_str(), msg.size(), 0);
		return (0);
	}

	if (_invite_only && std::find(_invited.begin(), _invited.end(), cli->get_fd()) == _invited.end())
	{
		std::string msg = return_cmd_failure(473, _name + " ", "Cannot join channel (+i not invited)"); 
		send(cli->get_fd(), msg.c_str(), msg.size(), 0);
		return (0);
	}

	if (!_key.empty() && _key != key)
	{
		std::string msg = return_cmd_failure(475, _name + " ", "Cannot join channel (+k wrong key)"); 
		send(cli->get_fd(), msg.c_str(), msg.size(), 0);
		return (0);
	}

	_members.push_back(cli);
	if (_members.size() == 1)
		addOp(cli);
	if (std::find(_invited.begin(), _invited.end(), cli->get_fd()) != _invited.end())
		_invited.erase(std::find(_invited.begin(), _invited.end(), cli->get_fd()));

	cli->add_channel(_name);
	std::string msg = return_cmd_success(cli, "JOIN", _name);
	send(cli->get_fd(), msg.c_str(), msg.size(), 0);
	broadcast(msg, cli->get_fd());
	return (1);
}

void Channel::rmMember(Client *cli)
{
	_members.erase(std::find(_members.begin(), _members.end(), cli));
	if (std::find(_operators.begin(), _operators.end(), cli->get_fd()) != _operators.end())
		_operators.erase(std::find(_operators.begin(), _operators.end(), cli->get_fd()));
	return ;
}

bool Channel::ismember(Client *cli) const {
	return (std::find(_members.begin(), _members.end(), cli) != _members.end());
}

int Channel::addOp(Client *cli)
{
	if (std::find(_members.begin(), _members.end(), cli) == _members.end())
	{
		std::string msg = return_cmd_failure(475,
			cli->get_nick() + _name + " ", "They aren't on that channel");
		return (send(cli->get_fd(), msg.c_str(), msg.size(), 0), 0);
	}
	_operators.push_back(cli->get_fd());
	std::string msg = return_msg_info(324, cli->get_nick(), _name + "+o");
	return (broadcast(msg, cli->get_fd()), 1);
}


int Channel::rmOp(Client *cli)
{
	if (std::find(_operators.begin(), _operators.end(), cli->get_fd()) == _operators.end())
	{
		std::string msg = return_cmd_failure(475,
			cli->get_nick() + _name + " ", "They aren't on that channel");
		return (send(cli->get_fd(), msg.c_str(), msg.size(), 0), 0);
	}
	_operators.erase(std::find(_operators.begin(), _operators.end(), cli->get_fd()));
	std::string msg = return_msg_info(324, cli->get_nick(), _name + "-o");
	return (broadcast(msg, cli->get_fd()), 1);
}

bool	Channel::isop(Client *cli) const{
	return (std::find(_operators.begin(), _operators.end(), cli->get_fd()) != _operators.end());
}

void Channel::addInvited(Client *cli)
{
	if (std::find(_invited.begin(), _invited.end(), cli->get_fd()) != _invited.end())
		_invited.push_back(cli->get_fd());
}

	// if (_invite_only
	// 	&& std::find(_operators.begin(), _operators.end(), cli->get_fd()) == _operators.end())
	// {
	// 	std::string msg = return_cmd_failure(451, "", "Permission Denied- You're not an IRC operator");
	// 	return (send(cli->get_fd(), msg.c_str(), msg.size(), 0), -1);
	// }

void Channel::broadcast(std::string msg, int fd) const
{
	const char *c_msg = msg.c_str();
	int	len_cmsg = msg.size();

	for (std::list<Client *>::const_iterator it = _members.begin(); it != _members.end(); it ++)
	{
		if ((*it)->get_fd() != fd)
			send((*it)->get_fd(), c_msg, len_cmsg, 0);
	}
}