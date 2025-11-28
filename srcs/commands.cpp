/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: earnera <earnera@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 12:00:42 by earnera           #+#    #+#             */
/*   Updated: 2025/11/28 11:58:46 by earnera          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

/* *************************CONSTRUCTEURS************************************* */

Commands::Commands() {}

/* **************************DESTRUCTEURS************************************** */

Commands::~Commands() {}

/* *******************************HANDLE*************************************** */

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


void Commands::handlePASS(Server &serv, Client* cli, const std::vector<std::string>& cmd){
    if(cmd.size() < 2)
    {
        std::cerr << "ERR_NEEDMOREPARAMS" << std::endl;
        return;
    }
    if(cli->_has_pass)
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
    cli->_has_pass = true;
}

void Commands::handleNICK(Server &serv, Client* cli, const std::vector<std::string>& cmd){

    if(cmd.size() < 2 || cmd[1].empty())
    {
        std::cerr << " ERR_NONICKNAMEGIVEN" << std::endl;
        return ;
    }
    
    std::string nickname = cmd[1];
    
    if(!isvalidnickname)
    {
        std::cerr << " ERR_ERRONEUS" << std::endl;
        return ;
    }
    Client *other = serv.get_client_by_nickname(nickname);
    if(other != NULL && other != cli){
        std::cerr << "ERR_NICKNAMEINUSE" << std::endl;
        return ;
    }
    
    cli->set_nick(nickname); 
    if(!cli->_has_nick)
        cli->_has_nick = true;
}
        //        ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
        //    ERR_NICKNAMEINUSE               ERR_NICKCOLLISION
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

