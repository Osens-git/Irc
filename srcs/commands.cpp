/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: earnera <earnera@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 12:00:42 by earnera           #+#    #+#             */
/*   Updated: 2025/11/27 15:16:36 by earnera          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

/* *************************CONSTRUCTEURS************************************* */

Commands::Commands() {}

/* **************************DESTRUCTEURS************************************** */

Commands::~Commands() {}

/* *******************************HANDLE*************************************** */

void Commands::handlePASS(Server &serv, Client* cli, const std::vector<std::string>& cmd){
    if(cmd.size() < 2)
    {
        std::cerr << "ERR_NEEDMOREPARAMS" << std::endl;
        return;
    }
    if(cli->hasPass)
    {
        std::cerr << "ERR_ALREADYREGISTERED" << std::endl;
        return;
    }
    std::string password = cmd[1];
    if(password != serv.get_pawd())
    {
        std::cerr << "Wrong password, try again" << std::endl;
        return ;
    }
    cli->hasPass = true;
}

void Commands::handleNICK(Server &serv, Client* cli, const std::vector<std::string>& cmd){
        (void)serv;
    (void)cli;
    (void)cmd;
}

void Commands::handleUSER(Server &serv, Client* cli, const std::vector<std::string>& cmd){
        (void)serv;
    (void)cli;
    (void)cmd;
}

void Commands::handlePRIVMSG(Server &serv, Client* cli, const std::vector<std::string>& cmd){
        (void)serv;
    (void)cli;
    (void)cmd;
}

void Commands::handleJOIN(Server &serv, Client* cli, const std::vector<std::string>& cmd){
        (void)serv;
    (void)cli;
    (void)cmd;
}

void Commands::handleQUIT(Server &serv, Client* cli, const std::vector<std::string>& cmd){
        (void)serv;
    (void)cli;
    (void)cmd;
}

void Commands::handlePART(Server &serv, Client* cli, const std::vector<std::string>& cmd){
        (void)serv;
    (void)cli;
    (void)cmd;
}

void Commands::handleKICK(Server &serv, Client* cli, const std::vector<std::string>& cmd){
        (void)serv;
    (void)cli;
    (void)cmd;
}

void Commands::handleINVITE(Server &serv, Client* cli, const std::vector<std::string>& cmd){
    (void)serv;
    (void)cli;
    (void)cmd;
}

void Commands::handleTOPIC(Server &serv, Client* cli, const std::vector<std::string>& cmd){
        (void)serv;
    (void)cli;
    (void)cmd;
}

void Commands::handleMODE(Server &serv, Client* cli, const std::vector<std::string>& cmd){
        (void)serv;
    (void)cli;
    (void)cmd;
}

