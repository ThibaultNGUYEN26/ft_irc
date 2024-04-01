/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/16 12:23:51 by thibnguy          #+#    #+#             */
/*   Updated: 2024/04/01 17:32:49 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <sstream>
#include <map>
#include <unistd.h>
#include <poll.h>
#include <vector>

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
	int _port;
	std::string _password;
	struct t_server	 _server;
	std::map<std::string, Client>	clients;

};
