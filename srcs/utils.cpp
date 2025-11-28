/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 16:10:29 by vluo              #+#    #+#             */
/*   Updated: 2025/11/28 20:45:41 by vluo             ###   ########.fr       */
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
	char buf[5 + nick.size() + cmd.size() + arg.size()];
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

*/
std::string	return_cmd_failure(Client *cli, int code, std::string arg, std::string msg)
{
	char buf[25 + arg.size() + msg.size()];
	sprintf(buf, ":ircserv %d %s :%s\n", code, arg.c_str(), msg.c_str());
	send(cli->get_fd(), buf, std::strlen(buf), 0);
	return (buf);
}

/*
For sending messages to inform client -> :ircserv <code> <nick> :<msg>

Exemple:
>	PASS pwd
>	NICK vluo
>	USER vl 0 *vl
->	:ircserv 001 vluo :Welcome to ircserv
->	:ircserv 002 vluo :Your host is ircserv
->	...

*/
std::string	return_msg_info(Client *cli, int code, std::string msg)
{
	std::string nick = cli->get_nick();
	char buf[25 + nick.size() + msg.size()];
	std::sprintf(buf, ":ircserv %d %s:%s\n", code, nick.c_str(), msg.c_str());
	send(cli->get_fd(), buf, std::strlen(buf), 0);
	return (buf);
}

void	print_parse(std::vector<std::string> parse)
{
	std::cout << parse[0] << std::endl;
	std::cout << "SIZE : " << parse.size() << std::endl;
	std::cout << "PARSING : ";
	for (unsigned long i = 0; i < parse.size(); i ++)
		std::cout << "|" << parse[i] << "| ";
	std::cout << "END" << std::endl;	
}

std::vector<std::string> split_cmd(std::string cmds)
{
	int deb = 0;
	unsigned long pos = cmds.find(' ');
	std::vector<std::string> parse;
	while (pos != std::string::npos)
	{
		parse.push_back(cmds.substr(deb, pos - deb));
		while (cmds[pos] && cmds[pos] == ' ')
			pos ++;
		deb = pos;
		pos = cmds.find(' ', deb);;
	}
	if (cmds.size() - deb - 2 != 0)
		parse.push_back(cmds.substr(deb, cmds.size() - deb - 2));

	// print_parse(parse);

	return (parse);
}