/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 14:55:36 by vluo              #+#    #+#             */
/*   Updated: 2025/11/30 11:18:32 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Channel.hpp"

#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <vector>

class Server {
	
	private :
	
		Server();
		Server(Server const &);
		Server &operator=(Server const &);

		int					_sock;
		sockaddr_in			_hint;
		int			 		_port;
		std::string	const 	_pawd;

	public :
	
		Server(char **argv);
		~Server();

		std::vector<Client *>	clients;
		std::vector<Channel *>	channels;
		int						max_fd;
		fd_set 					read_fd;
		int						stop;

		int					get_sock() const;
		sockaddr_in 		get_hint() const;
		int			 		get_port() const;
		std::string	const 	get_pawd() const;

		int					add_client();
		void				delete_client(int fd);
		Client				*get_client_by_fd(int fd);
		Client				*get_client_by_nick(std::string nick);
		
		int					create_channel(std::string name);
		void				delete_channel(std::string name);
		Channel				*get_Channel_by_name(std::string name);


};