/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 14:55:36 by vluo              #+#    #+#             */
/*   Updated: 2025/11/03 16:33:13 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netinet/in.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>

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

		int					get_sock() const;
		sockaddr_in 		get_hint() const;
		int			 		get_port() const;
		std::string	const 	get_pawd() const;

};