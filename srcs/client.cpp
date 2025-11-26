/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgelgon <cgelgon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 17:12:06 by vluo              #+#    #+#             */
/*   Updated: 2025/11/26 16:29:06 by cgelgon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "server.hpp"
#include "Channel.hpp"

Client::Client(){}
Client::Client(Client const &){} 

Client::Client(Server* server) {
	
	sockaddr_in cl;
	int cl_size = sizeof(struct sockaddr_in);

	_fd = accept(server->get_sock(), (struct sockaddr*)&cl, 
		(socklen_t*)&cl_size);

	if (_fd == -1) {
		std::cerr << "Error: Could not accept connection" << std::endl;
		return ;
	} 

	fcntl(_fd, F_SETFL, O_NONBLOCK);

	set_username(inet_ntoa(cl.sin_addr));
	set_nick(_username);
	// set_ch_id(0);
	// set_ch_id(0);
	this->_serverptr = server;
}
Client::~Client(){}

void	Client::set_nick(std::string const nick) { _nickname = nick; }
void	Client::set_username(std::string const username) { _username = username; }
// void	Client::set_ch_id(int const ch_id) { _ch_id = ch_id; }
// void	Client::set_ch_right(int const ch_right) { _ch_right = ch_right; }

std::string Client::get_nick() const { return (_nickname); }
std::string Client::get_usrname() const { return (_username); }
// int 		Client::get_ch_id() const { return (_ch_id); }
// int 		Client::get_ch_right() const { return (_ch_right); }
int			Client::get_fd() const { return (_fd); }

//CHANNEL USING : 
void Client::getClientChan(std::string chanName)
{
	const std::vector<Channel*>&all_chan = _serverptr->getChanList();
	authorized_chans.clear();

		for (size_t i = 0; i < all_chan.size(); i++)
		{
			if(all_chan[i]->isMember(this->_fd))
			{
				authorized_chans.push_back(all_chan[i]);
			}
		}
}
const std::vector<Channel*>& Client::getAuthorizedChans()const
{
	return this->authorized_chans;
}

void Client::chan_join(std::string chan_name)
{
	for (size_t i; i < authorized_chans.size(); i++)
	{
		if (authorized_chans[i]->getName() == chan_name)
			return;
	}
		Channel* chan = _serverptr->findChannel(chan_name);
		if (chan != NULL && chan->isMember(_fd))
			authorized_chans.push_back(chan);
}

void Client::chan_quit(std::string chan_name)
{
	for (size_t i; i < authorized_chans.size(); i++)
	{
		if (authorized_chans[i]->getName() == chan_name)
		{
			authorized_chans.erase(authorized_chans.begin() +1);
			return;
		}
	}
	
}


bool Client::isInChan(const std::string& chan_name) const
{
	for (size_t i; i < authorized_chans.size(); i++)
	{
		if (authorized_chans[i]->getName() == chan_name)
			return true;
	}
	return false;	
}
