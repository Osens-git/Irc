/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:29 by vluo              #+#    #+#             */
/*   Updated: 2025/11/30 17:40:57 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

int	g_signal = 0;

int	check_args(char **argv){

	char *p;
	std::strtold(argv[1], &p);

	if (*p != 0)
		return (std::cerr << "Error: " << argv[1] << " not valid port" << std::endl, 0);
	return (1);
}

void	check_iscmd(Server &serv, Client *cli)
{
	if (cli->buf == "")
		return ;

	std::string cmd = cli->buf;
	unsigned long pos = cli->buf.find(' ');
	if (pos != std::string::npos)
		cmd = cli->buf.substr(0, pos);
	cmd = to_upper(cmd);

	// if (cmd == "PASS")
	// 	std::vector<std::string> str = split_cmd(cli->buf);
	// if (cmd == "NICK")
	// if (cmd == "USER")

	// // if (!cli->registered())
	// {
	// 	return_cmd_failure(cli, 451, "", "You have not registered");
	// 	return ;
	// }

	if (cmd == "QUIT")
		handle_quit(serv, cli);
	if (cmd == "JOIN")
		handle_join(serv, cli);
	if (cmd == "PART")
		handle_part(serv, cli);
	if (cmd == "PRIVMSG")
		handle_privmsg(serv, cli);
	if (cmd == "KICK")
		handle_kick(serv, cli);
	if (cmd == "INVITE")
		handle_inivte(serv, cli);
	if (cmd == "TOPIC")
		handle_topic(serv, cli);
	if (cmd == "MODE")
		handle_mode(serv, cli);

	if (cmd != "QUIT")
		cli->buf.clear();
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
		cli->buf += line.substr(0, line.size() - 2);
		check_iscmd(serv, cli);
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
		return (serv.delete_client(fd));
	handle_mes(serv, fd, read_val, buf, serv.get_client_by_fd(fd));
}

int	handle_client(Server &serv){

	while (!g_signal && !serv.stop)
	{
		fd_set rfd = serv.read_fd;
		int i = serv.get_sock();
		int	sel = select(serv.max_fd + 1, &rfd, NULL, NULL, NULL);
		while (sel > 0 && i <= serv.max_fd)
		{			
			if (FD_ISSET(i, &rfd))
			{
				if (i == serv.get_sock())
				{
					if (serv.add_client() == -1)
						return (1);
				}
				else // messsage
					handle_input(serv, i);
				sel --;
			}
			i ++;
		}
	}
	return (0);
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

	return (handle_client(serv));
}