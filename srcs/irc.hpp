/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:55 by vluo              #+#    #+#             */
/*   Updated: 2025/11/28 18:47:04 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"

#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <fcntl.h>
#include <csignal>

std::string	return_cmd_success(Client *cli, std::string cmd, std::string arg);
std::string	return_cmd_failure(Client *cli, int code, std::string arg, std::string msg);
std::string	return_msg_info(Client *cli, int code, std::string msg);
void		print_parse(std::vector<std::string> parse); // to delte later
std::vector<std::string> split_cmd(std::string cmds);