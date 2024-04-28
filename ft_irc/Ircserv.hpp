/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/16 12:23:51 by thibnguy          #+#    #+#             */
/*   Updated: 2024/04/28 20:01:30 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "Errors.hpp"

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <algorithm>

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

typedef struct t_server {
	int sfd;
	sockaddr_in	hints;
	sockaddr_in	*res;
	std::vector<pollfd> fds;
} s_server;

typedef std::map<std::string, Client *> clientMap;
typedef std::map<std::string, Channel *> channelMap;

class Ircserv {

public:

	Ircserv(std::string &port, std::string &password);
	~Ircserv();

	void	initServer();
	void	runServer();
	void	eraseClient(int &clientSocket);
	bool	validateClientCommands(int& clientSocket, const std::string& _password);
	void	handleModeCommand(int clientSocket, const std::string& channelName, const std::string& modeSequence, const std::string& parameter, clientMap& clients, channelMap& channels);

private:

	int			_port;
	s_server	_server;
	std::string _password;
	clientMap	_clients;
	channelMap	_channels;
};

bool	isValidNickname(const std::string& nickname, clientMap& clients, int& clientSocket);

void	handleJoinCommand(int clientSocket, const std::string& channelName, const std::string& key, clientMap& clients, channelMap& channels);
void	handleLeaveCommand(int clientSocket, const std::string& channelName, clientMap& clients, channelMap& channels);
void	handleKickCommand(int clientSocket, const std::string& channelName, const std::string& userToKick, const std::string& reason, clientMap& clients, channelMap& channels);
void	handleTopicCommand(int clientSocket, const std::string& channelName, const std::string& newTopic, clientMap& clients, channelMap& channels);
void	handleInviteCommand(int clientSocket, const std::string& channelName, const std::string& guest, clientMap& clients, channelMap& channels);
void	broadcastToChannel(int senderSocket, const std::string& channelName, const std::string& message, clientMap& clients, channelMap& channels);
void	sendDM(int senderSocket, const std::string& channelName, const std::string& message, clientMap& clients);
