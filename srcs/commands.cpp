/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 13:16:43 by vluo              #+#    #+#             */
/*   Updated: 2025/11/29 13:46:32 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void	handle_join(Server &serv, Client *cli)
{
	std::vector<std::string> args = split(cli->buf, ' ');
	if (!enough_params(args, cli, 1))
		return ;

	std::vector<std::string> join_chan = split(args[1], ',');
	std::vector<std::string> join_key;
	if (args.size() > 2)
		join_key = split(args[2], ',');
	print_parse(join_chan);
	print_parse(join_key);
	(void)serv;
	return ;
}