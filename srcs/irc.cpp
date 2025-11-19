/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:29 by vluo              #+#    #+#             */
/*   Updated: 2025/11/19 15:45:08 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "server.hpp"

int	g_signal = 0;

int	check_args(char **argv){

	char *p;
	std::strtold(argv[1], &p);

	if (*p != 0)
		return (std::cerr << "Error: " << argv[1] << " not valid port" << std::endl, -1);
	return (1);
}

void	handle_client(Server &serv){

	FD_SET(serv.get_sock(), &serv.read_fd);
	FD_SET(serv.get_sock(), &serv.write_fd);

	serv.fds.push_back(serv.get_sock());
	while (!g_signal)
	{
		int	sel = select(FD_SETSIZE, &serv.read_fd, &serv.write_fd, NULL, NULL);
		if (sel > 0){
			for(std::size_t i = 0; i < serv.fds.size(); i ++)
			{
				if (serv.fds[i] > 0 && FD_ISSET(serv.fds[i], &serv.read_fd))
				{
					if (i == 0) // new client
						serv.add_client();
					else // messsage
						// handle_mes(clients, fds[i]);
						break ;
				}
			}
		}
	}
}

void	handle_sig(int sig)
{
	g_signal = sig;
	std::cout << std::endl;
}


int main(int argc, char **argv)
{
	signal(SIGINT, &handle_sig);
	signal(SIGQUIT, &handle_sig);

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