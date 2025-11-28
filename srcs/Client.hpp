/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 17:12:39 by vluo              #+#    #+#             */
/*   Updated: 2025/11/28 19:30:18 by vluo             ###   ########.fr       */
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
#include <arpa/inet.h>
#include <vector>
#include <algorithm>

#define BUFFER_SIZE 4096

class Client
{
	private :

		Client &operator=(Client const &);

		int							_fd;
		std::string					_nickname;
		std::string					_username;
		std::string					_host;
		std::vector<std::string>	_chans;

	public :

		Client();
		Client(Client const &);
		Client(int serv_fd);
		~Client();

		std::string					buf;
	
		void	set_nick(std::string const);
		void	set_username(std::string const);

		std::string	get_nick() 		const;
		std::string	get_usrname() 	const;
		std::string	get_host()		const;
		int			get_fd() 		const;

		void	delete_channel(std::string const name);
		void	add_channel(std::string const name);
		bool	is_inchannel(std::string const name) const;
		
};