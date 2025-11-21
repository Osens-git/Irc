/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:29 by vluo              #+#    #+#             */
/*   Updated: 2025/11/21 16:35:53 by vluo             ###   ########.fr       */
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

void	handle_mes(Server &serv, int fd, int read_val, char *buf)
{
	std::string line(buf, buf + read_val);

	int	rl = recv(fd, buf, BUFFER_SIZE, 0);
	while (rl > 0)
	{
		line += std::string(buf, buf + rl);
		rl = recv(fd, buf, BUFFER_SIZE, 0);
	}

	Client *cli = serv.get_client(fd);

	if (line.find('\n') != std::string::npos)
	{
		std::cout << cli->buf + line;
		send(fd, (cli->buf + line).c_str(), cli->buf.size() + line.size(), 0);
		cli->buf.clear();
		return ;
	}
	if (read_val > 0)
		cli->buf.append(line);
}

void	handle_input(Server &serv, int fd)
{
	char buf[BUFFER_SIZE];

	int	read_val = recv(fd, buf, BUFFER_SIZE, 0);
	if (read_val <= 0)
	{
		std::cout << "ircserv: Client " << fd << " disconnected" << std::endl;
		serv.delete_client(fd);
		return ;
	}
	else
	{
		handle_mes(serv, fd, read_val, buf);
	}
}

void	handle_client(Server &serv){

	while (!g_signal)
	{
		fd_set rfd = serv.read_fd;
		int i = 3;
		int	sel = select(serv.max_fd + 1, &rfd, NULL, NULL, NULL);
		while (sel > 0 && i <= serv.max_fd)
		{			
			if (FD_ISSET(i, &rfd))
			{
				if (i == serv.get_sock())
				{
					serv.add_client();
					sel --;
				}
				else // messsage
				{
					handle_input(serv, i);
					sel --;
				}
			}
			i ++;
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
		return (std::cerr << "Usage: ./ircserv <port> <password." << std::endl, -1);
	if (!check_args(argv))
		return (-1);

	Server serv(argv);
	if (serv.get_sock() == -1)
		return (-1);
	
	handle_client(serv);

	return (0);
}