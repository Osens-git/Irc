/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 13:16:43 by vluo              #+#    #+#             */
/*   Updated: 2025/11/30 18:25:59 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "irc.hpp"

void	handle_quit(Server &serv, Client *cli)
{
	unsigned long pos = cli->buf.find(" ");
	std::string quit_msg("");
	if (pos != std::string::npos)
		quit_msg = " " + cli->buf.substr(pos);
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

void	handle_join(Server &serv, Client *cli)
{
	std::vector<std::string> args = split(cli->buf, ' ');
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

void	handle_part(Server &serv, Client *cli)
{
	std::vector<std::string> args = split(cli->buf, ' ');
	if (!enough_params(args, cli, 1))
		return ;

	std::vector<std::string> join_chan = split(args[1], ',');
	std::string leaving_msg("");
	for (unsigned long i = 2 ; i < args.size(); i ++)
		leaving_msg.append(" " + args[i]);

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
		cli->delete_channel(name);
		ch->rmMember(cli);
		std::string msg = return_cmd_success(cli, "PART", name + leaving_msg);
		send(cli->get_fd(), msg.c_str(), msg.size(), 0);
		ch->broadcast(msg, cli->get_fd());
	}
}

void	handle_privmsg(Server &serv, Client *cli)
{
	std::vector<std::string> args = split(cli->buf, ' ');
	if (args.size() - 1 < 1)
		return (send_fail(cli,411, "", "No recipient given (PRIVMSG)"));

	std::string send_msg("");
	for (unsigned long i = 2 ; i < args.size() - 1; i ++)
		send_msg.append(args[i] + " ");
	send_msg.append(args[args.size() - 1]);

	Client *reci = serv.get_client_by_nick(args[1]);
	Channel *ch = serv.get_Channel_by_name(args[1]);
	
	if (reci == NULL && ch == NULL)
		return (send_fail(cli, 401, args[1] + " ", "No such nick/channel"));
	if (args.size() == 2)
		return (send_fail(cli, 412, "", "No text to send"));

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
		std::string msg = return_cmd_success(cli, "KICK", *it + kick_msg);
		ch->broadcast(msg, cli->get_fd());
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
		std::string msg = return_cmd_success(cli, "KICK", *it + kick_msg);
		ch->broadcast(msg, cli->get_fd());
		ch->rmMember(usr);
		usr->delete_channel(*it);
	}
}

void	handle_kick(Server &serv, Client *cli)
{
	std::vector<std::string> args = split(cli->buf, ' ');
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

void	handle_inivte(Server &serv, Client *cli)
{
	std::vector<std::string> args = split(cli->buf, ' ');
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

	std::string cli_msg = return_msg_info(341, cli->get_nick(),args[2] + " " + args[1]);
	send(cli->get_fd(), cli_msg.c_str(), cli_msg.size(), 0);

	ch->addInvited(usr);
}

void	handle_topic(Server &serv, Client *cli)
{
	std::vector<std::string> args = split(cli->buf, ' ');
	if (!enough_params(args, cli, 1))
		return ;

	Channel *ch = serv.get_Channel_by_name(args[1]);
	if (!ch)
		return (send_fail(cli, 442, args[1] + " ", "You're not on that channel"));

	if (args.size() == 2)
	{
		if (ch->getTopic() == "")
		{
			std::string msg = return_msg_info(331, cli->get_nick() + " " + args[1], "No topic is set");
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
	
	ch->setTopic(topic);
	std::string msg = return_cmd_success(cli, "TOPIC", topic);
	send(cli->get_fd(), msg.c_str(), msg.size(), 0);
}

void	handle_mode(Server &serv, Client *cli)
{
	std::vector<std::string> args = split(cli->buf, ' ');
	if (!enough_params(args, cli, 3))
		return ;

	Channel *ch = serv.get_Channel_by_name(args[1]);
	std::string mode = args[2];
	std::string params = args[3];

	if (!ch)
		return (send_fail(cli, 403, args[1], "No such channel"));
	if (!channop(ch, cli))
		return ;
	if (mode.size() != 2 || (mode[1] != 'i' && mode[1] != 't' && mode[1] != 'k' && mode[1] != 'o' && mode[1] != 'l'
			&& mode[0] != '+' && mode[0] != '-'))
		return (send_fail(cli, 472, std::string(&mode[1]) + " ", "is unknown mode char to me for " + args[1]));
	
	if (mode[1] == 'i')
	{
		ch->setInviteOnly(mode[0] == '+');
		std::string msg = return_msg_info(324, cli->get_nick(), args[1] + " " + mode);
		ch->broadcast(msg, cli->get_fd());
	}

	if (mode[1] == 't')
	{
		ch->setTopicRestricted(mode[0] == '+');
		std::string msg = return_msg_info(324, cli->get_nick(), args[1] + " " + mode);
		ch->broadcast(msg, cli->get_fd());
	}

	if (mode[1] == 'k')
	{
		if (mode[0] == '+')
		{
			if (ch->getKey() == "")
				return (send_fail(cli, 467, args[1] + " ", "Channel key already set"));
			std::string msg = return_msg_info(324, cli->get_nick(), args[1] + " " + mode + " " + params);
			ch->broadcast(msg, cli->get_fd());
			ch->setKey(params);
		}
		else
		{
			std::string msg = return_msg_info(324, cli->get_nick(), args[1] + " " + mode + " " + ch->getKey());
			ch->broadcast(msg, cli->get_fd());
			ch->setKey("");
		}
	}

	if (mode[1] == 'o')
	{
		Client *usr = serv.get_client_by_nick(params);
		if (!usr)
			return (send_fail(cli, 441, params + " " + args[1] + " ",  "They aren't on that channel"));

		if (mode[0] == '+')
			ch->addOp(usr);
		else
			ch->rmOp(usr);
		std::string msg = return_msg_info(324, cli->get_nick(), args[1] + " " + mode + " " + params);
		ch->broadcast(msg, cli->get_fd());
	}

	if (mode[1] == 'l')
	{
		if (mode[0] == '+')
		{
			int limit = std::atoi(params.c_str());
			if (limit == 0)
				return ;
			ch->setUserLimit(limit);
			std::string msg = return_msg_info(324, cli->get_nick(), args[1] + " " + mode + " " + params);
			ch->broadcast(msg, cli->get_fd());
		}
		else
		{
			std::string msg = return_msg_info(324, cli->get_nick(), args[1] + " " + mode);
			ch->broadcast(msg, cli->get_fd());
		}
	}
}