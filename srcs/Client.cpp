/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 17:12:06 by vluo              #+#    #+#             */
/*   Updated: 2025/12/03 17:31:40 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(){}
Client::Client(Client const &){} 

Client::Client(int server_fd) :_has_pass(false), _has_nick(false),
                                 _has_user(false), _registered(false) {
	
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

}
Client::~Client(){}

void	Client::set_nick(std::string const nick) { _nickname = nick; }
void	Client::set_username(std::string const username) { _username = username; }
void	Client::set_realname(std::string const realname) { _realname = realname; }
void	Client::set_mode(int mode) { _mode = mode; }

std::string Client::get_nick() const { return (_nickname); }
std::string Client::get_usrname() const { return (_username); }
std::string Client::get_host() const { return (_host); }
std::string Client::get_realname() const { return (_realname); }
int			Client::get_fd() const { return (_fd); }
int			Client::get_mode() const { return (_mode); }

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
