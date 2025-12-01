/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgelgon <cgelgon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 17:12:39 by vluo              #+#    #+#             */
/*   Updated: 2025/12/01 16:13:08 by cgelgon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstdio>
#include <errno.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
// #include <cstdlib>
// #include <stdlib.h>

#define BUFFER_SIZE 1

class Server;
class Channel;

class Client
{
	private :

		Client &operator=(Client const &);

		int				_fd;
		std::string		_nickname;
		std::string		_username;
		// int				_ch_id;		// channel id where the user is in (0 if not in any) , ...
		// int				_ch_right;	// ch_right 0 = operator, ch_right 1 = normal user , ...
		// CHANNEL USING : 
		Server* _serverptr;
		std::vector<Channel*> authorized_chans;
		
		public :
		
		Client();
		Client(Client const &);
		Client(Server* server);
		~Client();
		
		std::string	buf;
		
		void	set_nick(std::string const);
		void	set_username(std::string const);
		// void	set_ch_id(int const);
		// void	set_ch_right(int const);
		
		std::string	get_nick() 		const;
		std::string	get_usrname() 	const;
		// int			get_ch_id() 	const;
		// int			get_ch_right() 	const;
		int			get_fd() 		const;
		int			get_id()		const;
		
		// CHANNEL PART
		//void getClientChan(std::string chanName);
		const std::vector<Channel*>& getAuthorizedChans() const;
		void chan_join(std::string chan_name);
		void chan_quit(std::string chan_name);
		bool isInChan(const std::string& chan_name) const;
	
};