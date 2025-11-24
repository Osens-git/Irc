/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vluo <vluo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 18:50:55 by vluo              #+#    #+#             */
/*   Updated: 2025/11/24 15:32:00 by vluo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "client.hpp"
#include "server.hpp"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <algorithm>
#include <vector>
#include <fcntl.h>
#include <csignal>


