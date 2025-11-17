/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 17:12:39 by vluo              #+#    #+#             */
/*   Updated: 2025/11/15 12:02:32 by vluo             ###   ########.fr       */
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

class Client
{
	private :

		Client &operator=(Client const &);
	
		int 			_id; // arbitrary or not (like index 0 to nb_users or whatever he is identified by)
		int				_fd;
		std::string		_nickmane;
		std::string		_username;
		int				_ch_id;		// channel id where the user is in (0 if not in any) , ...
		int				_ch_right;	// ch_right 0 = operator, ch_right 1 = normal user , ...
	
	public :

		Client();
		Client(Client const &);
		Client(int id, int serv_fd);
		~Client();
	
		void	set_nick(std::string const);
		void	set_username(std::string const);
		void	set_ch_id(int const);
		void	set_ch_right(int const);

		std::string	get_nick() 		const;
		std::string	get_usrname() 	const;
		int			get_ch_id() 	const;
		int			get_ch_right() 	const;
		
};