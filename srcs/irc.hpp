/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:55 by vluo              #+#    #+#             */
/*   Updated: 2025/12/04 13:19:04 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Server.hpp"
// #include "Channel.hpp"

#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <fcntl.h>
#include <csignal>
#include <cctype>
#include <algorithm>

/* UTILS */
std::string					return_cmd_success(Client *cli, std::string cmd, std::string arg);
std::string					return_cmd_failure(Client *cli, int code, std::string arg, std::string msg);
std::string					return_msg_info(int code, std::string cmd, std::string msg);
void						print_parse(std::vector<std::string> parse); // to delete later
std::vector<std::string>	split(std::string cmds, char delimiter);
void						send_fail(Client *cli, int code, std::string arg, std::string msg);
std::string					to_upper(std::string str);
int							enough_params(std::vector<std::string> args, Client *cli, unsigned long nb_params);
int							channop(Channel *chan, Client *cli);
int							good_ch_mask(std::string name, Client *cli);

/* COMMANDS */
void	handle_pass(Server &serv, Client* cli, const std::vector<std::string>& cmd);
void	handle_nick(Server &serv, Client* cli, const std::vector<std::string>& cmd);
void	handle_user(Server &serv, Client* cli, const std::vector<std::string>& cmd);
void	handle_ping(Server &serv, Client *cli, const std::vector<std::string>& cmd);
void	handle_quit(Server &serv, Client *cli, std::string line);
void	handle_join(Server &serv, Client *cli, std::string line);
void	handle_part(Server &serv, Client *cli, std::string line);
void	handle_privmsg(Server &serv, Client *cli, std::string line);
void	handle_kick(Server &serv, Client *cli, std::string line);
void	handle_invite(Server &serv, Client *cli, std::string line);
void	handle_topic(Server &serv, Client *cli, std::string line);
void	handle_mode(Server &serv, Client *cli, std::string line);