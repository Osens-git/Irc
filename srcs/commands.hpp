/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: earnera <earnera@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 12:00:39 by earnera           #+#    #+#             */
/*   Updated: 2025/11/27 12:07:52 by earnera          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <iostream>
#include "server.hpp"
#include "client.hpp"

class Commands {
    public :
        Commands();
        ~Commands();
        static void handlePASS(Server &serv, Client* cli, const std::vector<std::string>& cmd);
        static void handleNICK(Server &serv, Client* cli, const std::vector<std::string>& cmd);
        static void handleUSER(Server &serv, Client* cli, const std::vector<std::string>& cmd);
        static void handlePRIVMSG(Server &serv, Client* cli, const std::vector<std::string>& cmd);
        static void handleJOIN(Server &serv, Client* cli, const std::vector<std::string>& cmd);
        static void handleQUIT(Server &serv, Client* cli, const std::vector<std::string>& cmd);
        static void handlePART(Server &serv, Client* cli, const std::vector<std::string>& cmd);
        static void handleKICK(Server &serv, Client* cli, const std::vector<std::string>& cmd);
        static void handleINVITE(Server &serv, Client* cli, const std::vector<std::string>& cmd);
        static void handleTOPIC(Server &serv, Client* cli, const std::vector<std::string>& cmd);
        static void handleMODE(Server &serv, Client* cli, const std::vector<std::string>& cmd);


        
    private :
} ;

#endif