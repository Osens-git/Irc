/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 17:12:06 by vluo              #+#    #+#             */
/*   Updated: 2025/11/19 17:49:48 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client(){}
Client::Client(Client const &){} 

Client::Client(int server_fd) {
	
	sockaddr_in cl;
	int cl_size = sizeof(struct sockaddr_in);

	_fd = accept(server_fd, (struct sockaddr*)&cl, 
		(socklen_t*)&cl_size);

	if (_fd == -1) {
		/* We have processed all incoming connections. */
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
			return ;
		}
		else {
			perror("Could not accept connection");
			return ;
		}
	} 

	/* Set the new socket to non-blocking */
	fcntl(_fd, F_SETFL, O_NONBLOCK);

	set_username(inet_ntoa(cl.sin_addr));
	set_nick(_username);
	// set_ch_id(0);
	// set_ch_id(0);

}
Client::~Client(){}

void	Client::set_nick(std::string const nick) { _nickmane = nick; }
void	Client::set_username(std::string const username) { _username = username; }
// void	Client::set_ch_id(int const ch_id) { _ch_id = ch_id; }
// void	Client::set_ch_right(int const ch_right) { _ch_right = ch_right; }

std::string Client::get_nick() const { return (_nickmane); }
std::string Client::get_usrname() const { return (_username); }
// int 		Client::get_ch_id() const { return (_ch_id); }
// int 		Client::get_ch_right() const { return (_ch_right); }
int			Client::get_fd() const { return (_fd); }
