/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: earnera <earnera@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 09:48:35 by earnera           #+#    #+#             */
/*   Updated: 2025/12/02 10:17:22 by earnera          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
#define REPLIES_HPP

#include "irc.hpp"
#include <string>
#include <iostream>

/*NUMERIC REPLIES*/
std::string RPL_WELCOME(const std::string& nick);
std::string RPL_TOPIC(const std::string& nick, const std::string& chan, const std::string topic);
std::string RPL_NOTOPIC(const std::string& nick, const std::string &chan);

/*ERRORS REPLIES*/
std::string ERR_NEEDMOREPARAMS(const std::string& chan);
std::string ERR_NEEDMOREPARAMS(const std::string& chan);        
std::string ERR_BANNEDFROMCHAN(const std::string& chan);
std::string ERR_BADCHANNELKEY(const std::string& chan);
std::string ERR_CHANNELISFULL(const std::string& chan);
std::string ERR_BADCHANMASK(const std::string& chan);
std::string ERR_NOSUCHCHANNEL(const std::string& chan);
std::string ERR_TOOMANYCHANNELS(const std::string& chan);

#endif