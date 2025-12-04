/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:29 by vluo              #+#    #+#             */
/*   Updated: 2025/12/04 13:30:14 by vluo             ###   ########.fr       */
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

int	check_iscmd(Server &serv, Client *cli, std::string line)
{
	std::cout << line << std::endl;

	std::vector<std::string> cmd_vec = split(line, ' ');
	if (cmd_vec.empty())
		return (0);

	std::string cmd = to_upper(cmd_vec[0]);

	if (cmd == "PASS")
		handle_pass(serv, cli, cmd_vec);
	else if (cmd == "NICK")
		handle_nick(serv, cli, cmd_vec);
	else if (cmd == "USER")
		handle_user(serv, cli, cmd_vec);
	else if (cmd == "PING")
		handle_ping(serv, cli, cmd_vec);

	if (!cli->_registered && cmd != "PASS" && cmd != "NICK" && cmd != "USER")
	{
		send_fail(cli, 451, "", "You have not registered");
		return (0);
	}

	if (cmd == "QUIT")
		return (handle_quit(serv, cli, line), 1);
	if (cmd == "JOIN")
		handle_join(serv, cli, line);
	if (cmd == "PART")
		handle_part(serv, cli, line);
	if (cmd == "PRIVMSG")
		handle_privmsg(serv, cli, line);
	if (cmd == "KICK")
		handle_kick(serv, cli, line);
	if (cmd == "INVITE")
		handle_invite(serv, cli, line);
	if (cmd == "TOPIC")
		handle_topic(serv, cli, line);
	if (cmd == "MODE")
		handle_mode(serv, cli, line);

	return (0);
}

void handle_mes(Server &serv, int read_val, char *buf, Client *cli)
{
	cli->buf.append(buf, read_val);

	int	stop = 0;
	size_t pos;
	while (!stop && (pos = cli->buf.find("\r\n")) != std::string::npos)
	{
		std::string line = cli->buf.substr(0, pos);
		cli->buf.erase(0, pos + 2);

		stop = check_iscmd(serv, cli, line);
	}
}


void	handle_input(Server &serv, int fd)
{
	char buf[BUFFER_SIZE];

	int	read_val = recv(fd, buf, BUFFER_SIZE, 0);

	if (read_val <= 0)
		return (serv.delete_client(fd));
	handle_mes(serv, read_val, buf, serv.get_client_by_fd(fd));
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