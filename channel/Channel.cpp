/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgelgon <cgelgon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 11:16:16 by cgelgon           #+#    #+#             */
/*   Updated: 2025/11/21 13:50:37 by cgelgon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <sys/socket.h>

Channel::Channel() : name(""), topic(""), key(""), invite_only(false), topic_restricted(false), user_limit(0)
{
}

Channel::Channel(const std::string& name) : name(name), topic(""), key(""), invite_only(false), topic_restricted(false), user_limit(0)
{
}

Channel::Channel(const Channel& other): name(other.name), topic(other.topic), key(other.key), members(other.members), operators(other.operators), invited(other.invited),
invite_only(other.invite_only), topic_restricted(other.topic_restricted), user_limit(other.user_limit)
{
}

Channel& Channel::operator=(const Channel&other)
{
	if (this != &other)
	{
		name = other.name;
		topic = other.topic;
		key = other.key;
		members = other.members;
		operators = other.operators;
		invited = other.invited;
		invite_only = other.invite_only;
		topic_restricted = other.topic_restricted;
		user_limit = other.user_limit;
	}
	return *this;
}

Channel::~Channel()
{}


//Getters
const std::string& Channel::getName() const
{
	return this->name;
}

const std::string& Channel::getTopic() const
{
	return this->topic;
}

const std::string& Channel::getKey() const
{
	return this->key;
}

bool Channel::isInviteOnly() const
{
	return invite_only;
}

bool Channel::isTopicRestricted() const
{
	return topic_restricted;
}

size_t Channel::getUserLimit() const
{
	return user_limit;
}

size_t Channel::getMemberCount() const
{
	return members.size();
}

const std::set<int>& Channel::getMembers() const
{
	return members;
}

//Setters
bool Channel::setTopic(const std::string& _topic, int fd)
{
	if (topic_restricted && !isOp(fd))
		return false;
	topic = _topic;
	return true;
}
bool Channel::setKey(const std::string& _key)
{
	key = _key;
}

void Channel::setInviteOnly(bool value)
{
	invite_only = value;
}

void Channel::setTopicRestricted(bool value)
{
	topic_restricted = value;
}

void Channel::setUserLimit(size_t limit)
{
	user_limit = limit;
}

// Methodes
int Channel::addMember(int fd)
{
	if (user_limit > 0 && members.size() >= user_limit)
	{
		std::cerr << "Error : channel full" << std::endl;
		return -1;	
	}
	if (invite_only && invited.count(fd) == 0)
	{
		std::cerr << "Error : not invited" << std::endl;
		return -1;
	}
	if (!key.empty() && key != provided_key)
	members.insert(fd);
	invited.erase(fd);
	return 0;
}

int Channel::rmMember(int fd)
{
	if (members.count(fd) == 0)
	{
		std::cerr << "Error : Client is not a member" << std::endl;
		return -1;	
	}
	members.erase(fd);
	if (operators.count(fd) == 1)
		operators.erase(fd);
	return 0;
}

bool Channel::isMember(int fd) const
{
	return(members.count(fd) > 0);
}

int Channel::addOp(int fd)
{
		if (members.count(fd) == 0)
	{
		std::cerr << "Error : Client cant be operator if not member" << std::endl;
		return -1;	
	}
	operators.insert(fd);
	return 0;
}


int Channel::rmOp(int fd)
{
	if (operators.count(fd) == 0)
	{
		std::cerr << "Error : Client is not a member" << std::endl;
		return -2;	
	}
	operators.erase(fd);
	return 0;
}

bool Channel::isOp(int fd) const
{
	return(operators.count(fd) > 0);
}

bool Channel::isInvited(int fd)
{
	return (invited.count(fd) > 0)
}

int Channel::addInvited(int fd)
{
	if (members.count(fd) == 1)
	{
		std::cerr << "Error : Client is already member" << std::endl;
		return -3;
	}
	invited.insert(fd);
	return 0;		
}

int Channel::broadcast(std::string msg, int exclude_fd)
{
	int count = 0;
	std::set<int>::iterator it;
	for (it = members.begin(); it != members.end(); ++it)
	{
		if (*it != exclude_fd)
		{
			send(*it, msg.c_str(), msg.size(), 0);
			count++;
		}
	}
	return count; // nb de msg sent
}

