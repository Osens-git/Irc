/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 13:16:43 by vluo              #+#    #+#             */
/*   Updated: 2025/12/03 18:04:28 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "irc.hpp"

std::string extract_full(const std::vector<std::string>& cmd)
{
	if(cmd.size() < 5)
		return "";
	std::string realname;
	for(size_t i = 4; i < cmd.size(); i++)
	{
		if(!realname.empty())
			realname += " ";
		realname += cmd[i];
	}
	if(!realname.empty() && realname[0] == ':')
		realname.erase(0, 1);
	return(realname);
}

bool        isvalidnickname(std::string nick){
    if(nick.empty())
        return false;
    const std::string specials = "`-[]^{}_|";
    
    char first = nick[0];
    if(!(std::isalpha(static_cast<unsigned char>(first)) || specials.find(first) != std::string::npos))
        return false;
    for(size_t i = 1; i < nick.size(); i++)
    {
        char c = nick[i];
        if(!(std::isalnum(static_cast<unsigned char>(c)) || specials.find(c) != std::string::npos))
            return false;
    }
    return true;
}


void handle_pass(Server &serv, Client* cli, const std::vector<std::string>& cmd)
{
    if (cmd.size() < 2)
    {
        send_fail(cli, 461, "PASS ", "Not enough parameters");
        return;
    }

    if (cli->_has_pass)
    {
        send_fail(cli, 462, "", "You may not reregister");
        return;
    }

    if (cmd[1] != serv.get_pawd())
    {
        send_fail(cli, 464, "", "Password incorrect");
        return;
    }

    cli->_has_pass = true;
	if(cli->_has_pass && cli->_has_nick && cli->_has_user)
		serv.register_client(cli);
}


void handle_nick(Server &serv, Client* cli, const std::vector<std::string>& cmd)
{
    if (cmd.size() < 2 || cmd[1].empty())
    {
        send_fail(cli, 431, "", "No nickname given");
        return;
    }

    std::string nickname = cmd[1];

    if (!isvalidnickname(nickname))
    {
        send_fail(cli, 432, nickname + " ", "Erroneous nickname");
        return;
    }

    Client* other = serv.get_client_by_nick(nickname);
    if (other && other != cli)
    {
        send_fail(cli, 433, nickname + " " + nickname + " ", "Nickname is already in use");
        return;
    }

    std::string oldnick = cli->get_nick();
    cli->set_nick(nickname);
    cli->_has_nick = true;
	if(cli->_has_pass && cli->_has_nick && cli->_has_user)
			serv.register_client(cli);
	if (cli->_has_nick)
	{
		std::string msg = ":" + oldnick + "!" + cli->get_usrname() + "@" + cli->get_host() + " NICK :" + nickname + "\r\n";
		send(cli->get_fd(), msg.c_str(), msg.size(), 0);
	}
		
}

void handle_user(Server &serv, Client* cli, const std::vector<std::string>& cmd)
{
	if (cmd.size() < 5)
    {
        send_fail(cli, 461, "USER ", "Not enough parameters");
        return;
    }

    if (cli->_has_user)
    {
        send_fail(cli, 462, "", "You may not reregister");
        return;
    }
	cli->set_username(cmd[1]);
	cli->set_mode(std::atoi(cmd[2].c_str()));
	cli->set_realname(extract_full(cmd));
    cli->_has_user = true;
	if(cli->_has_pass && cli->_has_nick && cli->_has_user)
		serv.register_client(cli);
}

void	handle_quit(Server &serv, Client *cli, std::string line)
{
	unsigned long pos = line.find(" ");
	std::string quit_msg("");
	if (pos != std::string::npos)
		quit_msg = line.substr(pos);
	for (std::vector<std::string>::iterator it = cli->chans.begin(); it < cli->chans.end(); it ++)
	{
		std::string msg = return_cmd_success(cli, "QUIT", *it + quit_msg);
		Channel *ch = serv.get_Channel_by_name(*it);
		ch->rmMember(cli);
		ch->broadcast(msg, cli->get_fd());
	}
	cli->chans.clear();
	std::string msg = return_cmd_success(cli, "QUIT", quit_msg);
	send(cli->get_fd(), msg.c_str(), msg.size(), 0);
	serv.delete_client(cli->get_fd());
	return ;
}

void	handle_join(Server &serv, Client *cli, std::string line)
{
	std::vector<std::string> args = split(line, ' ');
	if (!enough_params(args, cli, 1))
		return ;

	std::vector<std::string> join_chan = split(args[1], ',');
	std::vector<std::string> join_key;
	if (args.size() > 2)
		join_key = split(args[2], ',');

	unsigned long k = -1;
	for (std::vector<std::string>::iterator it = join_chan.begin(); it < join_chan.end(); it ++)
	{
		k ++;
		if (!good_ch_mask(*it, cli))
			continue ;

		Channel *ch = serv.get_Channel_by_name(*it);
		if (ch == NULL)
		{
			if (!serv.create_channel(*it))
				return ;
			(*(serv.channels.end() - 1))->addMember(cli, "");
			continue ;
		}
		if (join_key.empty() || k >= join_key.size())
			ch->addMember(cli, "");
		else
			ch->addMember(cli, join_key[k]);
	}
	return ;
}

void	handle_part(Server &serv, Client *cli, std::string line)
{

	std::cout << line << std::endl;

	std::vector<std::string> args;
	std::string leaving_msg("");
	std::size_t pos = line.find(':');
	if (pos != std::string::npos)
	{
		args = split(line.substr(0, pos - 1), ' ');
		leaving_msg = line.substr(pos + 1);
	}
	else
		args = split(line, ' ');

	if (!enough_params(args, cli, 1))
		return ;

	std::vector<std::string> join_chan = split(args[1], ',');

	for (std::vector<std::string>::iterator it = join_chan.begin(); it < join_chan.end(); it ++)
	{
		std::string name(*it);
		Channel *ch = serv.get_Channel_by_name(name);
		if (ch == NULL)
		{
			send_fail(cli, 403, name + " ", "No such channel");
			continue ;
		}
		if (!cli->is_inchannel(name))
		{
			send_fail(cli, 442, name + " ", "You're not on that channel");
			continue ;
		}
		std::string msg = return_cmd_success(cli, "PART " + name, leaving_msg);
		send(cli->get_fd(), msg.c_str(), msg.size(), 0);
		ch->broadcast(msg, cli->get_fd());
		cli->delete_channel(name);
		ch->rmMember(cli);
	}
}

void	handle_privmsg(Server &serv, Client *cli, std::string line)
{
	std::vector<std::string> args = split(line, ' ');
	if (args.size() - 1 < 1)
		return (send_fail(cli,411, "", "No recipient given (PRIVMSG)"));

	std::size_t pos1 = line.find(' ');
	std::size_t pos2 = line.substr(pos1 + 1).find(' ');
	std::string reci_name = line.substr(pos1 + 1, pos2);

	std::string send_msg("");
	if (pos2 != std::string::npos)
		send_msg = line.substr(pos1 + 1 + pos2 + 1);
	if (send_msg[0] == ':')
		send_msg.erase(0, 1);

	Client *reci = serv.get_client_by_nick(reci_name);
	Channel *ch = serv.get_Channel_by_name(reci_name);
	
	if (reci == NULL && ch == NULL)
		return (send_fail(cli, 401, args[1] + " ", "No such nick/channel"));
	if (send_msg == "")
		return (send_fail(cli, 412, " ", "No text to send"));

	std::string msg = return_cmd_success(cli, "PRIVMSG " + args[1], send_msg);
	if (reci != NULL)
		send(reci->get_fd(), msg.c_str(), msg.size(), 0);
	if (ch != NULL)
		ch->broadcast(msg, cli->get_fd());
}

static void kick_from_1chan(Server &serv, Client *cli, std::string ch_name, std::vector<std::string> kick_users, std::string kick_msg)
{
	if (!good_ch_mask(ch_name, cli))
		return ;
	Channel *ch = serv.get_Channel_by_name(ch_name);
	if (ch == NULL)
		return (send_fail(cli, 403, ch_name + " ", "No such channel"));
	if (!channop(ch, cli))
		return ;
	if (!ch->ismember(cli))
		return (send_fail(cli, 442, ch_name + " ", "You're not on that channel"));
	for (std::vector<std::string>::iterator it = kick_users.begin(); it < kick_users.end(); it ++)
	{
		Client *usr = serv.get_client_by_nick(*it);
		if (!ch->ismember(usr))
		{
			send_fail(cli, 441, *it + " " + ch_name + " ", "They aren't on that channel");
			continue ;
		}
		std::string msg = return_cmd_success(cli, "KICK " + ch_name, *it + kick_msg);
		std::cout << "msg: |" << msg << std::endl;
		ch->broadcast(msg, -1);
		ch->rmMember(usr);
		usr->delete_channel(ch_name);
	}
}

void	kick_chs_usrs(Server &serv, Client *cli, std::vector<std::string> kick_chans,
	std::vector<std::string> kick_users, std::string kick_msg)
{
	int u = -1;
	for (std::vector<std::string>::iterator it = kick_chans.begin(); it < kick_chans.end(); it ++)
	{
		u ++;
		if (!good_ch_mask(*it, cli))
			continue ;
		Channel *ch = serv.get_Channel_by_name(*it);
		if (ch == NULL)
		{
			send_fail(cli, 403, *it + " ", "No such channel");
			continue ;
		}
		if (!channop(ch, cli))
			continue ;
		if (!ch->ismember(cli))
		{
			send_fail(cli, 442, *it + " ", "You're not on that channel");
			continue ;
		}

		Client *usr = serv.get_client_by_nick(kick_users[u]);
		if (!ch->ismember(usr))
		{
			send_fail(cli, 441, kick_users[u] + " " + ch->getName() + " ", "They aren't on that channel");
			continue;
		}
		std::string msg = return_cmd_success(cli, "KICK " + ch->getName(), usr->get_nick() + " " + kick_msg);
		ch->broadcast(msg, cli->get_fd());
		ch->rmMember(usr);
		usr->delete_channel(*it);
	}
}

void	handle_kick(Server &serv, Client *cli, std::string line)
{

	std::cout << "buf : |" << line << std::endl;

	std::vector<std::string> args = split(line, ' ');
	if (!enough_params(args, cli, 2))
		return ;

	std::vector<std::string>	kick_chans = split(args[1], ',');
	std::vector<std::string>	kick_users  = split (args[2], ',');
	std::string kick_msg("");
	if (args.size() > 3)
	{
		for (unsigned long i = 3 ; i < args.size(); i ++)
			kick_msg.append(" " + args[i]);
	}

	if (kick_chans.size() != kick_users.size() && kick_chans.size() != 1)
		return (send_fail(cli, 461, "KICK ", "Not enough parameters"));

	if (kick_chans.size() == 1)
		return (kick_from_1chan(serv, cli, kick_chans[0], kick_users, kick_msg));
	kick_chs_usrs(serv, cli, kick_chans, kick_users, kick_msg);
}

void	handle_inivte(Server &serv, Client *cli, std::string line)
{
	std::vector<std::string> args = split(line, ' ');
	if (!enough_params(args, cli, 2))
		return ;

	Client *usr = serv.get_client_by_nick(args[1]);
	Channel *ch = serv.get_Channel_by_name(args[2]);

	if (!usr)
		return (send_fail(cli, 401, args[1] + " ", "No such nick/channel"));
	if (!ch)
		return (send_fail(cli, 401, args[2] + " ", "No such nick/channel"));

	if (!ch->ismember(cli))
		return (send_fail(cli, 442, args[2] + " ", "You're not on that channel"));
	if (ch->ismember(usr))
		return (send_fail(cli, 443, args[1] + " " + args[2] + " ", "is already on channel"));

	if (!channop(ch, cli))
		return ;
	
	std::string usr_msg = return_cmd_success(cli, "INVITE " + args[1], args[2]);
	send(usr->get_fd(), usr_msg.c_str(), usr_msg.size(), 0);

	std::string cli_msg = return_msg_info(341, cli->get_nick() + " " + args[1], args[2]);
	send(cli->get_fd(), cli_msg.c_str(), cli_msg.size(), 0);

	ch->addInvited(usr);
}

void	handle_topic(Server &serv, Client *cli, std::string line)
{
	std::vector<std::string> args = split(line, ' ');
	if (!enough_params(args, cli, 1))
		return ;

	Channel *ch = serv.get_Channel_by_name(args[1]);
	if (!ch)
		return (send_fail(cli, 442, args[1] + " ", "You're not on that channel"));

	if (args.size() == 3 && args[2] == ":")
		args.pop_back();

	if (args.size() == 2)
	{
		if (ch->getTopic() == "")
		{
			std::string msg = return_msg_info(331, cli->get_nick(), args[1] + " :" "No topic is set");
			std::cout << msg << std::endl;
			send(cli->get_fd(), msg.c_str(), msg.size(), 0);
			return ;
		}
		std::string msg = return_msg_info(332, cli->get_nick() + " " + args[1], ch->getTopic());
		send(cli->get_fd(), msg.c_str(), msg.size(), 0);
		return ;
	}

	if (!channop(ch, cli))
		return ;

	std::string topic("");
	for (unsigned long i = 2; i < args.size() - 1; i ++)
		topic.append(args[i] + " ");
	topic.append(args[args.size() - 1]);
	if (topic[0] == ':')
		topic.erase(0, 1);
	
	ch->setTopic(topic);
	std::string msg = return_cmd_success(cli, "TOPIC " + ch->getName(), topic);
	ch->broadcast(msg, -1);
}

void	handle_mode(Server &serv, Client *cli, std::string line)
{
	std::vector<std::string> args = split(line, ' ');
	if (!enough_params(args, cli, 2))
		return ;

	Channel *ch = serv.get_Channel_by_name(args[1]);
	std::string mode = args[2];
	std::string params("");
	if (args.size() > 3) 
		params = args[3];

	if (!ch)
		return (send_fail(cli, 403, args[1] + " ", "No such channel"));
	if (!channop(ch, cli))
		return ;
	if (mode.size() < 2)
		return (send_fail(cli, 472, std::string(&mode[1]) + " ", "is unknown mode char to me for " + args[1]));

	std::cout << "mode : |" << mode << std::endl;
	std::cout << "params : |" << params << std::endl;

	int stop = mode.size();
	if (params != "")
		stop = std::min(mode.size() - 1, (unsigned long)3);
	for (unsigned long i = 1; i < mode.size(); i ++)
	{
		if (mode[i] != 'i' && mode[i] != 't' && mode[i] != 'k' && mode[i] != 'o' && mode[i] != 'l')
		{
			send_fail(cli, 472, mode[i] + std::string(" "), "is unknown mode char to me for " + args[1]);
			continue;
		}
		if (mode[i] == 'i')
		{
			ch->setInviteOnly(mode[0] == '+');
			continue ;
		}

		if (mode[i] == 't')
		{
			ch->setTopicRestricted(mode[0] == '+');
			continue ;
		}

		if (mode[i] == 'k')
		{
			if (mode[0] == '+')
			{
				if (ch->getKey() != "")
				{
					send_fail(cli, 467, args[1] + " ", "Channel key already set");
					continue ;
				}
				ch->setKey(params);
				continue ;
			}
			else
			{
				ch->setKey("");
				continue ;
			}
		}

		if (mode[i] == 'o')
		{
			if (params == "")
				continue ;
			Client *usr = serv.get_client_by_nick(params);
			if (!usr)
			{
				send_fail(cli, 441, params + " " + args[1] + " ",  "They aren't on that channel");
				continue ;
			}
			if (mode[0] == '+')
				ch->addOp(usr);
			else
				ch->rmOp(usr);
			continue;
		}

		if (mode[i] == 'l')
		{
			if (mode[0] == '+')
			{
				if (params == "")
				{
					send_fail(cli, 461, "MODE +l ", "Not enought parameters");
					continue ;
				}
				int limit = std::atoi(params.c_str());
				if (limit == 0)
					continue;
				ch->setUserLimit(limit);
				continue ;
			}
			else
			{
				ch->setUserLimit(-1);
				continue ;
			}
		}
	}
	if (params != "")
		params.insert(0, " ");

	std::string msg_cli = return_cmd_success(cli, "MODE " + args[1], mode.substr(0, stop + 1) + params);
	std::cout << msg_cli << std::endl;
	send(cli->get_fd(), msg_cli.c_str(), msg_cli.size(), 0);
	ch->broadcast(msg_cli, cli->get_fd());
}