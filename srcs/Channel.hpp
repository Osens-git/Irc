/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgelgon <cgelgon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 10:42:08 by cgelgon           #+#    #+#             */
/*   Updated: 2025/11/26 14:25:47 by cgelgon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <iostream>
#include <vector>
#include <set>


// class member : public Client
// {
// };

// class operators : public Client
// {};

class Channel
{
	private :
				std::string name;
				std::string topic;
				std::string key; //password
				std::set<int> members; // les int ici sont des fd concretement
				std::set<int> operators;
				std::set<int> invited;
				bool invite_only;
				bool topic_restricted;
				size_t user_limit;

				Channel();
				
				
				public :
				// Cannonique
				Channel(const std::string& name);
				~Channel();
				Channel(const Channel& other);
				Channel& operator=(const Channel& other);
				
				//Setters
				const std::string& getTopic() const;
				const std::string&	getName() const;
				const std::string&	getKey() const;
				bool isInviteOnly() const;
				bool isTopicRestricted() const;
				size_t getUserLimit() const;
				size_t getMemberCount() const;
				const std::set<int>& Channel::getMembers() const;

				// Setters				
				bool            setTopic(const std::string& topic, int fd);
				bool            setKey(const std::string& key);
				void			setInviteOnly(bool value);
				void			setTopicRestricted(bool value);
				void			setUserLimit(size_t limit);				
				
				// Methodes 
				// Members handling 
				int addMember(int fd, const std::string& provided_key = "");
				int rmMember(int fd);
				bool isMember(int fd) const;
				// Operators handling
				int addOp(int fd);
				int rmOp(int fd);
				bool isOp(int fd) const;
				// Invited handling 
				int addInvited(int fd);
				bool isInvited(int fd);
				// Send to all except exclude fd
				int broadcast(std::string msg, int exclude_fd);
				
};
			
			

#endif