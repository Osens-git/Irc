/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 15:02:51 by vluo              #+#    #+#             */
/*   Updated: 2025/11/17 19:26:41 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include <netdb.h>

Server::Server(char **argv) : _pawd(argv[2]){

	struct protoent *pe = getprotobyname("tcp");
	_sock = socket(AF_INET, SOCK_STREAM, pe->p_proto);
	if (_sock == -1)
	{
		std::cerr << "Error: Cannot create listening socket" << std::endl;
		return ;
	}

	long double port = std::strtold(argv[1], NULL);
	_hint.sin_family = AF_INET;
	_hint.sin_port = htons(port);
	_hint.sin_addr.s_addr = INADDR_ANY;

	_port = ntohs(_hint.sin_port);

	if (bind(_sock, (struct sockaddr *)&_hint, sizeof(_hint)) == -1)
	{
		close(_sock);
		std::cerr << "Error: Cannot bind socket to "
			<< port << " port" << std::endl;
		_sock = -1;
		return ;
	}
	
	if (listen(_sock, SOMAXCONN) == -1)
	{
		close(_sock);
		std::cerr << "Error: Cannot listen on created socket"
			<< std::endl;
		_sock = -1;
		return ;
	}
			
	fcntl(_sock, F_SETFL, O_NONBLOCK);
	if (_sock == -1)
	{
		std::cerr << "Error: Cannot create server" << std::endl;
		return ;
	}

	FD_ZERO(&read_fd);
	FD_ZERO(&write_fd);

	std::cout << "listening on port :" << _port << std::endl;
}
Server::~Server(){
	std::cout << "Server shutting down" << std::endl;

	for(unsigned long i = 0; i < clients.size(); i ++)
		delete clients[i];

}


int	Server::get_sock() const { return _sock; }
sockaddr_in Server::get_hint() const { return _hint; }
int Server::get_port() const { return _port; }
std::string const Server::get_pawd() const { return _pawd; }
