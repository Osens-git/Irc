/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgelgon <cgelgon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:29 by vluo              #+#    #+#             */
/*   Updated: 2025/12/01 17:18:23 by cgelgon          ###   ########.fr       */
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
	if (chan->getMemberCount() == 0)
		server.deleteChannel(chan_name);
}

void cmd_PRIVMSG(Server &server, Client *client, std::vector<std::string> &params)
{
	if (params.size() < 2)
	{
		send_reply(client->get_fd(), "ERROR need more parameters");
		return;
	}
	std::string target = params[0];
	std::string message = params[1];

	if (target[0] == '#')
	{
		Channel *chan = server.findChannel(target);
		if (chan == NULL)
		{
			send_reply(client->get_fd(), "ERROR : cham doesnt exist");
			return;
		}
		if (!chan->isMember(client->get_fd()))
		{
			send_reply(client->get_fd(), "ERROR : Cant send the message");
			return;
		}
		std::string msg = ":" + client->get_nick() + " PRIVMSG" + target + " :" + message + "\r\n";
		chan->broadcast(msg, client->get_fd());
	}
}

void cmd_KICK(Server &server, Client *client, std::vector<std::string> &params)
{
	if (params.size() < 2)
	{
		send_reply(client->get_fd(), "ERROR need more parameters");
		return;
	}
	std::string chan_name = params[0];
	std::string target_nick = params[1];
	std::string reason = (params.size() > 2) ? params[2] : "Kicked";

	Channel *chan = server.findChannel(chan_name);
	if (chan == NULL)
	{
		send_reply(client->get_fd(), "ERROR : You're not channel operator");
		return;
	}
	Client *target = NULL;
	for (size_t i = 0; i < server.clients.size(); i++)
	{
		if (server.clients[i]->get_nick() == target_nick)
		{
			target = server.clients[i];
			break;
		}
	}
	if (target == NULL)
	{
		send_reply(client->get_fd(), "ERROR : No such nick");
		return;
	}
	if (!chan->isMember(target->get_fd()))
	{
		send_reply(client->get_fd(), "ERROR : user not in channel");
		return;
	}
	std::string kick_msg = "+" + client->get_nick() + " KICK " + chan_name + " " + target_nick + " :" + reason + "\r\n";
	chan->broadcast(kick_msg, -1);
	chan->rmMember(target->get_fd());
	target->chan_quit(chan_name);
}

void cmd_INVITE(Server &server, Client *client, std::vector<std::string> &params)
{
	if (params.size() < 2)
	{
		send_reply(client->get_fd(), "ERROR need more parameters");
		return;
	}
	std::string target_nick = params[0];
	std::string chan_name = params[1];
	Channel *chan = server.findChannel(chan_name);
	if (chan == NULL)
	{
		send_reply(client->get_fd(), "ERROR : Channel doesnt exist");
		return;
	}
	if (!chan->isMember(client->get_fd()))
	{
		send_reply(client->get_fd(), "ERROR : you're not on that channel");
		return;		
	}

	Client *target = NULL;
	for (size_t i = 0; i < server.clients.size(); i++)
	{
		if (server.clients[i]->get_nick() == target_nick)
		{
			target = server.clients[i];
			break;
		}
	}
	if (target == NULL)
	{
		send_reply(client->get_fd(), "ERROR : no such nick");
		return;
	}
	if (chan->isMember(target->get_fd()))
	{
		send_reply(client->get_fd(), "ERROR : User already in channel");
		return;
	}
	chan->addInvited(target->get_fd());
	send_reply(client->get_fd(), "341" + client->get_nick() + " " + target_nick + " " + chan_name);
	std::string invite_msg = ":" + client->get_nick() + " INVITE " + target_nick + " " + chan_name;
	send_reply(target->get_fd(), invite_msg);
}

std::vector<std::string> split_params(const std::string& str)
{
		std::vector<std::string> result;
		size_t start = 0;
		size_t end = 0;
	
		while(end != std::string::npos)
		{
			end = str.find(' ', start);
			if (start < str.length() && str[start] == ':')
			{
				result.push_back(str.substr(start + 1));
				break;
			}
			std::string token = str.substr(start, (end = std::string::npos) ? std::string::npos : end - start);
			if (!token.empty())
				result.push_back(token);
			start = (end > std::string::npos) ? std::string::npos: end + 1;
		}
	return result;
}

void exec_cmd(Server &server, Client *client, const std::string& message)
{
	//takeoff \r\n
	std::string clean_msg = message;
	size_t pos = clean_msg.find("\r\n");
	if (pos != std::string::npos)
		clean_msg = clean_msg.substr(0, pos);
		
	// lf cmd
	size_t space_pos = clean_msg.find(' ');
	std::string cmd;
	std::string params_str;
	
	if (space_pos == std::string::npos)
			cmd = clean_msg;
	else
	{
		cmd = clean_msg.substr(0, space_pos);
		params_str = clean_msg.substr(space_pos + 1);
	}

	// parse param with split params
	std::vector<std::string> params = split_params(params_str);

	if (cmd == "JOIN")
		cmd_JOIN(server, client, params);
	else if (cmd == "PART")
		cmd_PART(server, client, params);
	else if (cmd == "PRIVMSG")
		cmd_PRIVMSG(server, client, params);
	else
		send_reply(client->get_fd(), "ERROR : Unknown cmd");
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