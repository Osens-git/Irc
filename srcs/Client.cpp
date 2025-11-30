/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 17:12:06 by vluo              #+#    #+#             */
/*   Updated: 2025/11/30 15:39:49 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(){}
Client::Client(Client const &){} 

Client::Client(int server_fd) {
	
	sockaddr_in cl;
	int cl_size = sizeof(struct sockaddr_in);

	_fd = accept(server_fd, (struct sockaddr*)&cl, 
		(socklen_t*)&cl_size);

	if (_fd == -1) {
		std::cerr << "Error: Could not accept connection" << std::endl;
		return ;
	} 

	fcntl(_fd, F_SETFL, O_NONBLOCK);
	_host = inet_ntoa(cl.sin_addr);
	char buf[14];
	sprintf(buf, "%d", _fd);
	_nickname = buf;
	// _username = "";

}
Client::~Client(){}

void	Client::set_nick(std::string const nick) { _nickname = nick; }
void	Client::set_username(std::string const username) { _username = username; }

std::string Client::get_nick() const { return (_nickname); }
std::string Client::get_usrname() const { return (_username); }
std::string Client::get_host() const { return (_host); }
int			Client::get_fd() const { return (_fd); }

void	Client::add_channel(std::string const name) { chans.push_back(name); }
void	Client::delete_channel(std::string const name) {
		chans.erase(std::find(chans.begin(), chans.end(), name)); }
bool	Client::is_inchannel(std::string const name) const {

	std::string up_name = to_upper(name);
	for (std::vector<std::string>::const_iterator it = chans.begin(); it < chans.end(); it ++)
	{
		if (to_upper(*it) == up_name)
			return (true);
	}
	return (false);
}
