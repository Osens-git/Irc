/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 16:10:29 by vluo              #+#    #+#             */
/*   Updated: 2025/12/03 17:59:30 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

/*
When the command is successful return no code except -> :<nick> <cmd> :<arg>

Exemple :
>	join #1 
->	:vluo JOIN :#1

>	part #1
->	:vluo PART :#1

(not every command success return a msg)

*/
std::string	return_cmd_success(Client *cli, std::string cmd, std::string arg)
{
	std::string nick = cli->get_nick();
	char buf[6 + nick.size() + cmd.size() + arg.size()];
	sprintf(buf, ":%s %s :%s\n", nick.c_str(), cmd.c_str(), arg.c_str());
	return (buf);
}

/*
When the command fails return a code -> :ircserv <code> <arg> :<msg>

Exemple:
> 	join #1 (if not registered)
->	:ircserv 451 :You have not registered

>	pass
->	:ircserv 461 PASS :Not enought parameters

(arg can be empty depending on the command)
//	also if <arg> is    	: "exemple"
//		pass <arg> + " "	: "exemple " 
//		  if <arg> is		: ""
//		pass <arg> as is	: ""
//	to respect syntax 		

*/
std::string	return_cmd_failure(Client *cli, int code, std::string arg, std::string msg)
{
	char buf[25 + cli->get_nick().size() + arg.size() + msg.size()];
	if (cli->get_nick() == "")
		sprintf(buf, ":ircserv %03d %s:%s\n", code, arg.c_str(), msg.c_str());
	else
		sprintf(buf, ":ircserv %03d %s %s:%s\n", code, cli->get_nick().c_str(), arg.c_str(), msg.c_str());
	return (buf);
}

/*
For sending messages to inform client -> :ircserv <code> <cmd> :<msg>

Exemple:
>	PASS pwd
>	NICK vluo
>	USER vl 0 *vl
->	:ircserv 001 vluo :Welcome to ircserv
->	:ircserv 002 vluo :Your host is ircserv
->	...

*/
std::string	return_msg_info(int code, std::string cmd, std::string msg)
{
	char buf[25 + cmd.size() + msg.size()];
	std::sprintf(buf, ":ircserv %03d %s :%s\n", code, cmd.c_str(), msg.c_str());
	return (buf);
}

void	print_parse(std::vector<std::string> parse)
{
	std::cout << "SIZE : " << parse.size() << std::endl;
	std::cout << "PARSING : ";
	for (unsigned long i = 0; i < parse.size(); i ++)
		std::cout << "|" << parse[i] << "| ";
	std::cout << "END" << std::endl;	
}

std::vector<std::string> split(std::string cmds, char delimiter)
{
	int deb = 0;
	unsigned long pos = cmds.find(delimiter);
	std::vector<std::string> parse;
	while (pos != std::string::npos)
	{
		parse.push_back(cmds.substr(deb, pos - deb));
		while (cmds[pos] && cmds[pos] == delimiter)
			pos ++;
		deb = pos;
		pos = cmds.find(delimiter, deb);;
	}
	if (cmds.size() - deb > 0)
		parse.push_back(cmds.substr(deb, cmds.size() - deb));

	// print_parse(parse);

	return (parse);
}

void	send_fail(Client *cli, int code, std::string arg, std::string msg)
{
	std::string rtr = return_cmd_failure(cli, code, arg, msg);
	send(cli->get_fd(), rtr.c_str(), rtr.size(), 0);
	std::cout << rtr ;
}

std::string	to_upper(std::string str)
{
	std::string res(str);
	for (std::size_t i = 0; i < str.size(); i ++)
		res[i] = std::toupper(str[i]);
	return (res);
}

int	enough_params(std::vector<std::string> args, Client *cli, unsigned long nb_params)
{
	if (args.size() - 1 < nb_params)
	{
		std::string msg = return_cmd_failure(cli, 461, args[0] + " ", "Not enough parameters");
		send(cli->get_fd(), msg.c_str(), msg.size(), 0);
		return (0);
	}
	return (1);
}
int	channop(Channel *chan, Client *cli)
{
	if (!chan->isop(cli))
	{
		std::string msg = return_cmd_failure(cli, 481, "", "Permission Denied- You're not an IRC operator");
		send(cli->get_fd(), msg.c_str(), msg.size(), 0);
		return (0);
	}
	return (1);
}

int	good_ch_mask(std::string name, Client *cli)
{
	if (name.size() > 50 || (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] != '!'))
	{
		std::string msg = return_cmd_failure(cli, 476, name + " ", "Bad Channel Mask");
		send(cli->get_fd(), msg.c_str(), msg.size(), 0);
		return (0);
	}
	for (unsigned long int i = 0; i < name.size(); i ++)
	{
		if (name[i] == 7)
		{
			std::string msg = return_cmd_failure(cli, 476, name + " ", "Bad Channel Mask");
			std::cout << msg << std::endl;
			send(cli->get_fd(), msg.c_str(), msg.size(), 0);
			return (0);
		}
	}
	return (1);		
}