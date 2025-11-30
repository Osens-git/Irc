/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 15:02:51 by vluo              #+#    #+#             */
/*   Updated: 2025/11/30 17:03:05 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "irc.hpp"

Server::Server(char **argv) :_pawd(argv[2]), stop(0) {

	struct protoent *pe = getprotobyname("tcp");
	_sock = socket(AF_INET, SOCK_STREAM, pe->p_proto);
	if (_sock == -1)
	{
		std::cerr << "Error: Cannot create listening socket" << std::endl;
		return ;
	}

	const int opt = 1;
	if(setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == -1)
	{
		std::cerr << "Error: Could not set socket options" << std::endl;
		close(_sock);
		_sock = -1;
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
	max_fd = _sock;

	std::cout << "ircserv: Listening on port : <" << _port << ">" << std::endl;
}
Server::~Server(){
	std::cout << "ircserv: Server shutting down" << std::endl;

	for(unsigned long i = 0; i < clients.size(); i ++)
	{
		close(clients[i]->get_fd());
		FD_CLR(clients[i]->get_fd(), &read_fd);
		delete clients[i];
	}
	clients.clear();
	for (unsigned long i = 0; i < channels.size(); i ++)
		delete channels[i];
	channels.clear();
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
		return (0);
	}
		
	std::cout << "ircserv: Client " << c->get_fd() << " connected to server" <<std::endl;
	clients.push_back(c);
	FD_SET(c->get_fd(), &read_fd);
	if (c->get_fd() > max_fd)
		max_fd = c->get_fd();

	// std::string msg(":ircserv: 001 vluo :Welcome");
	// send(c->get_fd(), msg.c_str(), msg.size(), 0);
		
	return (1);
}

void	Server::delete_client(int fd){

	Client *cli = get_client_by_fd(fd);
	std::cout << "ircserv: Client " << fd << " disconnected" << std::endl; 
	close(fd);
	FD_CLR(fd, &read_fd);
	clients.erase(std::find(clients.begin(), clients.end(), cli));
	delete cli;
}

Client *Server::get_client_by_fd(int fd){
	for(unsigned long i = 0; i < clients.size(); i ++)
		if (clients[i]->get_fd() == fd)
			return (clients[i]);
	return (NULL);
}
Client *Server::get_client_by_nick(std::string nick){
	for(unsigned long i = 0; i < clients.size(); i ++)
		if (clients[i]->get_nick() == nick)
			return (clients[i]);
	return (NULL);
}

int	Server::create_channel(std::string name)
{
	Channel *chan = new Channel(name);
	if (chan == 0)
	{
		std::cerr << "Error: Failed to allocate new memory for client" << std::endl;
		stop = 1;
		return (0);
	}
	
	channels.push_back(chan);
	return (1);
}

void	Server::delete_channel(std::string name)
{
	for (std::vector<Channel *>::iterator it = channels.begin(); it < channels.end(); it ++)
	{
		if ((*it)->getName() == name)
		{
			delete *it;
			channels.erase(it);
			return ;
		}
	}
}

Channel	*Server::get_Channel_by_name(std::string name)
{
	std::string up_name = to_upper(name);
	for (std::vector<Channel *>::iterator it = channels.begin(); it < channels.end(); it ++)
	{
		if (to_upper((*it)->getName()) == up_name)
			return (*it);
	}
	return (NULL);
}
