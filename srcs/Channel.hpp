/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 10:42:08 by cgelgon           #+#    #+#             */
/*   Updated: 2025/11/28 19:35:50 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"

#include <string>
#include <list>
#include <algorithm>

class Channel
{
	private :

		std::string 			_name;
		std::string 			_topic;
		std::string 			_key;
		std::list<Client *>		_members; 
		std::list<int> 			_operators;
		std::list<int> 			_invited;
		bool 					_invite_only;
		bool 					_topic_restricted;
		size_t					_user_limit;

		Channel& operator=(const Channel& other);

	public :
	
		Channel();
		Channel(const Channel&);
		Channel(const std::string& name);
		~Channel();
		
		//Getters
		std::string			getTopic()			const;
		std::string			getName()			const;
		std::string			getKey()			const;
		bool				isInviteOnly()		const;
		bool				isTopicRestricted() const;
		size_t				getUserLimit()		const;
		size_t				getMemberCount()	const;

		// Setters				
		void	setTopic(std::string topic);
		void	setKey(std::string key);
		void	setInviteOnly(bool value);
		void	setTopicRestricted(bool value);
		void	setUserLimit(size_t limit);

		// Methodes 

		// Members handling 
		int		addMember(Client *cli, std::string key);
		void	rmMember(Client *cli);
		bool	ismember(Client *cli) const;

		// Operators handling
		int		addOp(Client *cli);
		int		rmOp(Client *cli);
		bool	isop(Client *cli) const;
	
		// Invited handling 
		void	addInvited(Client *cli);
		
		// Send to all except exclude fd
		void	broadcast(std::string msg, int exclude_fd) const;
				
};

std::string	return_cmd_success(Client *cli, std::string cmd, std::string arg);
std::string	return_cmd_failure(Client *cli, int code, std::string arg, std::string msg);
std::string	return_msg_info(Client *cli, int code, std::string msg);
