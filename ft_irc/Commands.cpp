/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 14:55:06 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/26 20:07:17 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void	handleJoinCommand(int clientSocket, const std::string& channelName, const std::string& key, clientMap& clients, channelMap& channels) {
	// Get client nickname
	std::string nickname;
	clientMap::iterator itClient = clients.begin();
	
	while (itClient != clients.end()) {
		if ((itClient->second)->getSocket() == clientSocket) {
			nickname = (itClient->second)->getNickname();
			break;
		}
		++itClient;
	}
	// Check if the channel exists, create it if not
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		channels[channelName] = new Channel(channelName);
		(channels[channelName])->addClient(clientSocket);
		(itClient->second)->setOperator(true);
	}
	else {
		if (!((itChannel->second)->getKey().empty()) && key != (itChannel->second)->getKey()) {
			std::string joinFail = ":localhost 475 " + nickname + " " + channelName + " :Cannot join channel (+k) - incorrect key\r\n";
			send(clientSocket, joinFail.c_str(), joinFail.size(), 0);
			return ;
		}
		if ((itChannel->second)->getUserLimit() != -1 && (itChannel->second)->getUserLimit() < (itChannel->second)->getUsers())
		{
			std::string joinFail = ":localhost 471 " + nickname + " " + channelName + " :Cannot join channel (+l)\r\n";
			send(clientSocket, joinFail.c_str(), joinFail.size(), 0);
			return ;
		}
		(itChannel->second)->addClient(clientSocket);
		(itClient->second)->setOperator(false);
	}
	// Notify all clients in the channel about the new member
	// && Send JOIN message back to the client to confirm
	std::string joinConfirm = ":" + nickname + "!~user@host JOIN :" + channelName + "\r\n";
	send(clientSocket, joinConfirm.c_str(), joinConfirm.size(), 0);
	// Inform the other clients
	std::vector<int>& members = (channels[channelName])->getClients();
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != clientSocket) {
			send(*memberIt, joinConfirm.c_str(), joinConfirm.size(), 0);
		}
	}
}

void	handleInviteCommand(int clientSocket, const std::string& channelName, const std::string& key, clientMap& clients, channelMap& channels) {
	// Check if the person kicking has operator privilege
	std::string nickname;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			if ((it->second)->getOperator() == false) {
				std::string	kickFail = "localhost 482 " + nickname + " " + channelName + " :You're not on channel operator\r\n";
				send(clientSocket, kickFail.c_str(), kickFail.size(), 0);
				return ;
			}
			nickname = (it->second)->getNickname();
			break;
		}
	}
}
