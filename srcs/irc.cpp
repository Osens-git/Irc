/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:29 by vluo              #+#    #+#             */
/*   Updated: 2025/11/22 17:54:16 by vluo             ###   ########.fr       */
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

std::vector<std::string> split_cmd(std::string cmds)
{
	int deb = 0;
	unsigned long pos = cmds.find(' ');
	std::vector<std::string> parse;
	while (pos != std::string::npos)
	{
		parse.push_back(cmds.substr(deb, pos).c_str());
		while (cmds[pos] && cmds[pos] == ' ')
			pos ++;
		deb = pos;
		pos = cmds.find(' ', deb);;
	}
	parse.push_back(cmds.substr(deb, pos).c_str());
	return (parse);
}

void	check_iscmd(Server &serv, Client *cli)
{

	int pos = cli->buf.find(' ');
	std::string cmd = cli->buf.substr(0, pos);

	if (cmd == "PASS" || cmd == "PASS\r\n")
	{
		std::cout << "PASS" << std::endl;
		std::vector<std::string> str = split_cmd(cli->buf);
		std::cout << "SIZE : " << str.size() << std::endl;
		std::cout << "PARSING : ";
		for (unsigned long i = 0; i < str.size(); i ++)
			std::cout << "|" << str[i] << "| ";
		std::cout << "END" << std::endl;
	}
	// if (cmd == "NICK")
	// if (cmd == "USER")
	// if (cmd == "PRIVMSG")
	// if (cmd == "JOIN")
	// if (cmd == "QUIT")
	// if (cmd == "PART")
	// if (cmd == "KICK")
	// if (cmd == "INVITE")
	// if (cmd == "TOPIC")
	// if (cmd == "MODE")

	return ;
}

void	handle_mes(Server &serv, int fd, int read_val, char *buf, Client *cli)
{
	std::string line(buf, buf + read_val);

	int	rl = recv(fd, buf, BUFFER_SIZE, 0);
	while (rl > 0)
	{
		line += std::string(buf, buf + rl);
		rl = recv(fd, buf, BUFFER_SIZE, 0);
	}

	if (line.find("\r\n") != std::string::npos)
	{
		cli->buf += line;
		check_iscmd(serv, cli);
		// send(fd, (cli->buf + line).c_str(), cli->buf.size() + line.size(), 0);
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
		handle_mes(serv, fd, read_val, buf, serv.get_client(fd));
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