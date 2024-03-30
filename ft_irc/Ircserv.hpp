/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/16 12:23:51 by thibnguy          #+#    #+#             */
/*   Updated: 2024/03/30 15:48:51 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <sstream>

#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define EOC "\033[1;1;0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"

#define BACKLOG 10

struct t_server {
	int sfd;
	struct addrinfo	hints;
	struct addrinfo	*res;
};

class Ircserv {
	public:

		Ircserv(std::string &port, std::string &password);
		~Ircserv();
		
		void initServer();
		void runServer();

	private:
		struct t_server _server;
		int _port;
		std::string _password;

};
