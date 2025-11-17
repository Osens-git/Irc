/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:29 by vluo              #+#    #+#             */
/*   Updated: 2025/11/17 19:30:52 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "server.hpp"
#include "client.hpp"

int	check_args(char **argv){

	char *p;
	std::strtold(argv[1], &p);

	if (*p != 0)
		return (std::cerr << "Error: " << argv[1] << " not valid port" << std::endl, -1);
	return (1);
}

int	add_client(std::vector<Client *> &clients, std::vector<int> &fds)
{

	Client *c = new Client (clients.size(), fds[0]);
	std::cout << "Client " << c->get_fd() << " connected to server" <<std::endl;
	// std::cout << "client username: " << c->get_usrname() << std::endl;
	// std::cout << "client nickname: " << c->get_nick() << std::endl;
	clients.push_back(c);
	fds.push_back(c->get_fd());

	return (1);
}



int	handle_client(Server &serv){

	FD_SET(serv.get_sock(), &serv.read_fd);
	FD_SET(serv.get_sock(), &serv.write_fd);

	serv.fds.push_back(serv.get_sock());
	while (1)
	{
		int	sel = select(FD_SETSIZE, &serv.read_fd, &serv.write_fd, NULL, NULL);
		if (sel > 0){
			for(std::size_t i = 0; i < serv.fds.size(); i ++)
			{
				if (serv.fds[i] > 0 && FD_ISSET(serv.fds[i], &serv.read_fd))
				{
					if (i == 0) // new client
					{
						add_client(serv.clients, serv.fds);
						// std::cout << "C FDS: ";
						// for (unsigned long i = 0; i < clients.size(); i ++)
						// 	std::cout << clients[i]->get_fd() << " ";
						// std::cout << std::endl;

						// std::cout << "FDS: ";
						// for (unsigned long i = 0; i < fds.size(); i ++)
						// 	std::cout << fds[i] << " ";
						// std::cout << std::endl;
					}
					else // messsage
						// handle_mes(clients, fds[i]);
						break ;
				}
			}
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 3 )
		return (std::cerr << "Error: Wrong number of arguments" << std::endl, -1);
	if (!check_args(argv))
		return (-1);

	Server serv(argv);
	if (serv.get_sock() == -1)
		return (-1);

	handle_client(serv);
	
	close(serv.get_sock());
	return (0);
}