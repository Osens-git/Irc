/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 17:12:06 by vluo              #+#    #+#             */
/*   Updated: 2025/11/15 12:14:45 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client(){}
Client::Client(Client const &){} 

Client::Client(int id, int server_fd) : _id(id) {
	
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
			_fd = -1;
			return ;
		}
	} 

	/* Set the new socket to non-blocking */
	fcntl(_fd, F_SETFL, O_NONBLOCK);

	char user[5] = "User";
	char nick[14] = "User_Nickname";
	int info = getaddrinfo(user, nick, NULL, NULL);
	if (info != 0)
		std::cerr << "Could not get client info" << std::endl;
	_username = user;
	_nickmane = nick;
	set_ch_id(0);
	set_ch_id(0);

	std::cout << get_usrname() << std::endl;
	std::cout << get_nick() << std::endl;
	
}
Client::~Client(){}

void	Client::set_nick(std::string const nick) { _nickmane = nick; }
void	Client::set_username(std::string const username) { _username = username; }
void	Client::set_ch_id(int const ch_id) { _ch_id = ch_id; }
void	Client::set_ch_right(int const ch_right) { _ch_right = ch_right; }

std::string Client::get_nick() const { return (_nickmane); }
std::string Client::get_usrname() const { return (_username); }
int 		Client::get_ch_id() const { return (_ch_id); }
int 		Client::get_ch_right() const { return (_ch_right); }
