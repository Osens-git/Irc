/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgelgon <cgelgon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 14:55:36 by vluo              #+#    #+#             */
/*   Updated: 2025/11/26 14:48:22 by cgelgon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netinet/in.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <vector>

class Client;
class Channel;

class Server {
	
	private :
	
		Server();
		Server(Server const &);
		Server &operator=(Server const &);

		int					_sock;
		sockaddr_in			_hint;
		int			 		_port;
		std::string	const 	_pawd;
		// CHANNEL USING
		std::vector<Channel*> channels;
	public :
	
		Server(char **argv);
		~Server();

		std::vector<Client *>	clients;
		int						max_fd;
		fd_set 					read_fd;

		int					get_sock() const;
		sockaddr_in 		get_hint() const;
		int			 		get_port() const;
		std::string	const 	get_pawd() const;

		int					add_client();
		void				delete_client(int fd);
		Client				*get_client(int fd);
		
		// CHANNEL USING
		const std::vector<Channel*>& getChanList() const;
		Channel* createChannel(const std::string& name, int maker_fd);
		Channel* findChannel(const std::string& name);
		bool deleteChannel(const std::string& name);
		
};