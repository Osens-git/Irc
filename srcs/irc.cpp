/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:29 by vluo              #+#    #+#             */
/*   Updated: 2025/11/17 13:06:13 by vluo             ###   ########.fr       */
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

int	add_client(std::deque<Client> &clients, std::vector<int> fds)
{
	Client c(clients.size(), fds[0]);
	std::cout << "client username" << clients[0].get_usrname() << std::endl;
	std::cout << "client nickname" << clients[0].get_nick() << std::endl;
	clients.push_back(c);
	return (1);
}

int	handle_client(Server &serv){

	std::deque<Client>	clients;
	std::vector<int>	fds;
	fd_set read_fd;
	fd_set write_fd;

	fds.push_back(serv.get_sock());
	std::cout << "handle client " << std::endl;
	while (1)
	{
		FD_ZERO(&read_fd);
		int	sel = select(1, &read_fd, &write_fd, NULL, NULL);
		if (sel > 0){
			for(std::size_t i = 0; i < fds.size(); i ++)
			{
				if (fds[i] > 0 && FD_ISSET(fds[i], &read_fd))
				{
					if (i == 0) // new client
					{
						std::cout << "HELLO" << std::endl;
						add_client(clients, fds);
						// break ;
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