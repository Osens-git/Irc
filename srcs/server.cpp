/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 15:02:51 by vluo              #+#    #+#             */
/*   Updated: 2025/11/19 18:12:04 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Server::Server(char **argv) :_pawd(argv[2]), max_fd(3){

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
	FD_SET(_sock, &read_fd);

	std::cout << "ircserv: Listening on port : <" << _port << ">" << std::endl;
}
Server::~Server(){
	std::cout << "ircserv: Server shutting down" << std::endl;

	for(unsigned long i = 0; i < clients.size(); i ++)
	{
		FD_CLR(clients[i]->get_fd(), &read_fd);
		close(clients[i]->get_fd());
		delete clients[i];
	}
	if (_sock > 0)
		close(_sock);
}


int	Server::get_sock() const { return _sock; }
sockaddr_in Server::get_hint() const { return _hint; }
int Server::get_port() const { return _port; }
std::string const Server::get_pawd() const { return _pawd; }

int	Server::add_client()
{
	Client *c = new Client (_sock);
	if (c->get_fd() < 0 || c == NULL)
	{
		if (c == NULL)
			std::cerr << "Error: Failed to allocate \
				new memory for client" << std::endl;
		return (-1);
	}
		
	std::cout << "ircserv: Client " << c->get_fd() << " connected to server" <<std::endl;
	clients.push_back(c);
	FD_SET(c->get_fd(), &read_fd);
	if (c->get_fd() > max_fd)
		max_fd = c->get_fd();

	return (1);
}

void	Server::delete_client(int fd){

	for(unsigned long i = 0 ; i < clients.size(); i ++)
	{
		if (clients[i]->get_fd() == fd)
		{
			close(fd);
			FD_CLR(fd, &read_fd);
			delete clients[i];
			clients.erase(clients.begin() + i);
			break;
		}
	}
}

Client *Server::get_client(int fd){
	for(unsigned long i = 0; i < clients.size(); i ++)
		if (clients[i]->get_fd() == fd)
			return (clients[0]);
	return (NULL);
}
