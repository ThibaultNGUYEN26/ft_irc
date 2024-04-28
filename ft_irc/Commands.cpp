/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 14:55:06 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/28 20:10:43 by rchbouki         ###   ########.fr       */
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
	// Check if channel format is correct
	if (channelName[0] != '#') {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	// Check if the channel exists, create it if not
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		channels[channelName] = new Channel(channelName);
		(channels[channelName])->addClient(clientSocket);
		(itClient->second)->addChannel(channelName, true);
	}
	else {
		if (!((itChannel->second)->getKey().empty()) && key != (itChannel->second)->getKey()) {
			return ERRINCORRECTKEY(nickname, channelName, clientSocket);
		}
		if ((itChannel->second)->getUserLimit() != -1 && (itChannel->second)->getUserLimit() < (itChannel->second)->getUsers())
		{
			return ERRUSERLIMIT(nickname, channelName, clientSocket);
		}
		std::cout << (itClient->second)->getIsInvited(channelName) << std::endl;
		if ((itChannel->second)->getInviteOnly() && !(itClient->second)->getIsInvited(channelName)) {
			return ERRINVITEONLY(nickname, channelName, clientSocket);
		}
		(itChannel->second)->addClient(clientSocket);
		(itClient->second)->addChannel(channelName, false);
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
	std::string	topic = "";
	if (itChannel != channels.end()) {
		topic = (itChannel->second)->getTopic();
	}
	if (topic.empty()) {
		RPL_NOTOPIC(nickname, channelName, clientSocket);
	}
	else {
		RPL_TOPIC(nickname, channelName, topic, clientSocket);
	}
}

void	handleInviteCommand(int clientSocket, const std::string& channelName, const std::string& guest, clientMap& clients, channelMap& channels) {
	// Check if the person kicking has operator privilege
	std::string			nickname;
	clientMap::iterator	it = clients.begin();
	while (it != clients.end()) {
		if ((it->second)->getSocket() == clientSocket) {
			if ((it->second)->getOperator(channelName) == false) {
				return ERRNOTOPERATOR(nickname, channelName, clientSocket);
			}
			nickname = (it->second)->getNickname();
			break;
		}
		++it;
	}
	if (it == clients.end()) {
		return ERRNOTONCHANNEL(nickname, channelName, clientSocket);
	}
	// Check if the channel exists
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	// Find the user to kick based on their nickname
	int userSocket;
	it = clients.begin();
	while (it != clients.end()) {
		if (it->second->getNickname() == guest) {
			userSocket = it->second->getSocket();
			break;
		}
		++it;
	}
	if (it == clients.end()) {
		return ERRCLIENTUNKNOWN(guest, channelName, clientSocket);
	}
	// Check if the user is in the channel
	std::vector<int>& members = (itChannel->second)->getClients();
	std::vector<int>::iterator pos = std::find(members.begin(), members.end(), userSocket);
	if (pos != members.end()) {
		return ERRINCHANNEL(nickname, guest, channelName, clientSocket);
	}
	RPL_INVITING(nickname, guest, channelName, clientSocket);
	INVITE_MESSAGE(nickname, guest, channelName, userSocket);
	if ((itChannel->second)->getInviteOnly() == true) {
		(it->second)->setInvite(channelName, true);
	}
}
