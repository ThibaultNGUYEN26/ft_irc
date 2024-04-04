/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/16 12:23:51 by thibnguy          #+#    #+#             */
/*   Updated: 2024/04/04 15:34:21 by thibnguy         ###   ########.fr       */
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
#include <fcntl.h>
#include <arpa/inet.h>

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
	sockaddr_in	hints;
	sockaddr_in	*res;
	// Array of pollfd structures for monitoring file descriptors
	std::vector<struct pollfd> fds;
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

void passCommand(std::string &_password, struct t_server &_server, int &clientSocket);
void nickCommand(struct t_server &_server, int &clientSocket);	
void handleRegistration(int sockfd);
