/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcUtils.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 18:07:57 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 16:25:32 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sys/socket.h>

#include "Client.hpp"
#include "Channel.hpp"

#define EOC "\033[1;1;0m"
#define MAGENTA "\033[1;35m"

#define HOSTNAME "Titi&Riri"

typedef std::map<std::string, Client *> clientMap;
typedef std::map<std::string, Channel *> channelMap;

/* ERR AND RPL MESSAGES */
void	ERRUNKNOWNCOMMAND(const int& clientSocket, const std::string& nickname, const std::string& cmd);
void	ERRMOREPARAMS(const int& clientSocket, const std::string& nickname, const std::string& cmd);
void	ERRINCORRECTPASSWORD(const int& clientSocket);
void	ERRNICKLENGTH(const std::string& nickname, const int& clientSocket);
void	ERRNICKNAMEINUSE(const std::string& nickname, const int& clientSocket);
void	WELCOME_001(const std::string& nickname, const int& clientSocket);
void	ERRNOSUCHCHANNEL(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRNOTONCHANNEL(const std::string& nickname, const std::string& channelName, const int& 
clientSocket);
void	ERRNOTINCHANNEL(const std::string& nickname, const std::string& other, const std::string& channelName, const int& clientSocket);
void	ERRCLIENTUNKNOWN(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRINCORRECTKEY(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRUSERLIMIT(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRNOTOPERATOR(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRINCHANNEL(const std::string& nickname, const std::string& guest, const std::string& channelName, const int& clientSocket);
void	ERRINVITEONLY(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	RPL_INVITING(const std::string& nickname, const std::string& guest, const std::string& channelName, const int& clientSocket);
void	INVITE_MESSAGE(const std::string& guest, const std::string& channelName, const int& clientSocket);
void	RPL_NOTOPIC(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	RPL_TOPIC(const std::string& nickname, const std::string& channelName, const std::string& topic, const int& clientSocket);

/* UTILITY FUNCTIONS */
bool	isValidNickname(const std::string& nickname, clientMap& clients, int& clientSocket);
int		getUserSocket(const std::string& nickname, clientMap& clients);
void	broadcastToChannel(int senderSocket, const std::string& channelName, const std::string& message, clientMap& clients, channelMap& channels);
void	sendDM(int senderSocket, const std::string& target, const std::string& message, clientMap& clients);
const clientMap::iterator	getClientIterator(const int& clientSocket, clientMap& clients);
bool	checkNC(std::string& str);
