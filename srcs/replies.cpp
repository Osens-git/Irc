/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: earnera <earnera@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 09:38:54 by earnera           #+#    #+#             */
/*   Updated: 2025/12/02 10:20:22 by earnera          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "replies.hpp"
#include "irc.hpp"

namespace Replies {
    
/*NUMERIC REPLIES*/
std::string RPL_WELCOME(const std::string& nick)
{
    return ("001 " + nick + " :Welcome to this IRC server\r\n");
}
std::string RPL_TOPIC(const std::string& nick, const std::string& chan, const std::string topic)
{
    return ("332 " + nick + " " + chan + " :" + topic + "\r\n");
}

std::string RPL_NOTOPIC(const std::string& nick, const std::string &chan)
{
    return ("331 " + nick + " " + chan + " :No topic is set\r\n");
};

/*ERRORS REPLIES*/
std::string ERR_NEEDMOREPARAMS(const std::string& chan)
{
    return("461 " + chan + " :Not enough parameters\r\n");
}
std::string ERR_BANNEDFROMCHAN(const std::string& chan)
{
    return("474 " + chan + " :Cannot join channel\r\n");
}
std::string ERR_BADCHANNELKEY(const std::string& chan)
{
    return("475" + chan + " :Cannot join channel\r\n");
}
std::string ERR_CHANNELISFULL(const std::string& chan)
{
    return("471" + chan + " :Cannot join channel\r\n");
}
std::string ERR_BADCHANMASK(const std::string& chan)
{
    return("476" + chan + " :Bad Channel Mask\r\n");
}
std::string ERR_NOSUCHCHANNEL(const std::string& chan)
{
    return("403" + chan + " :No such channel\r\n");
}
std::string ERR_TOOMANYCHANNELS(const std::string& chan)
{
    return("405" + chan + " :You have joined too many channels\r\n");
}

}