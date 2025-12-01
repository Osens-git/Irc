/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgelgon <cgelgon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:29 by vluo              #+#    #+#             */
/*   Updated: 2025/12/01 14:40:24 by cgelgon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "Channel.hpp"

int	g_signal = 0;

int	check_args(char **argv){

	char *p;
	std::strtold(argv[1], &p);

	if (*p != 0)
		return (std::cerr << "Error: " << argv[1] << " not valid port" << std::endl, -1);
	return (1);
}

void send_reply(int fd, const std::string& message)
{
	std::string reply = message + "\r\n";
	send(fd, reply.c_str(), reply.size(), 0);
}

void cmd_JOIN(Server &server, Client *client, std:: vector<std::string> &params)
{
	if (params.size() < 1)
	{
		send_reply(client->get_fd(), "ERROR : not enough params");
		return;
	}
	std::string chan_name = params[0];
	std::string key = (params.size() < 1) ? params[1] : "";
	
	if (chan_name.empty() || chan_name[0] != '#')
	{
		send_reply(client->get_fd(), "ERROR : Bad channel name");
		return;
	}
	// lf chan, create it otherwise
	Channel *chan = server.findChannel(chan_name);
	if (chan == NULL)
	{
		chan = server.createChannel(chan_name, client->get_fd());
		if (chan == NULL)
		{
			send_reply(client->get_fd(), "ERROR : cant create channel");
			return;
		}
	client->chan_join(chan_name);
	//inform client
	std::string join_msg = ":" + client->get_nick() + " Joined the server";
	send_reply(client->get_fd(), join_msg);
	//send topic
	send_reply(client->get_fd(), "332 " + client->get_nick() + " " + chan_name + "No topic set");
	return;
	}
	//try to join chan if exist
	int result = chan->addMember(client->get_fd(), key);
	if (result == -1)
	{
		send_reply(client->get_fd(), "ERROR : Cannot join channel");
		return;
	}
	client->chan_join(chan_name);

	// get all member informed
	std::string join_msg = ":" + client->get_nick() + " Joined the server";
	chan->broadcast(join_msg + "\r\n", - 1);
	// send topic
	if (!chan->getTopic().empty())
		send_reply(client->get_fd(), "332" + client->get_nick() + " " + chan_name + chan->getTopic());
}

void cmd_PART(Server &server, Client *client, std::vector<std::string> &params)
{
	if (params.size() <  2)
	{
		send_reply(client->get_fd(), "ERROR need more parameters");
		return;
	}
	std::string chan_name = params[0];
	Channel *chan = server.findChannel(chan_name);
	if (chan == NULL)
	{
		send_reply(client->get_fd(), "ERROR : chan doesnt exist");
		return;
	}
	if (!chan->isMember(client->get_fd()))
	{
		send_reply(client->get_fd(), "ERROR : Must be member first to leave a chan");
		return;
	}
	std::string part_message = ":" + client->get_nick() + " PART " + chan_name;
	chan->broadcast(part_message + "\r\n", +1);
	chan->rmMember(client->get_fd());
	client->chan_quit(chan_name);
	if (chan->getMemberCount() == 0);
	server.deleteChannel(chan_name);
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