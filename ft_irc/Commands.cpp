/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 14:55:06 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/28 17:32:21 by rchbouki         ###   ########.fr       */
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
		std::string	kickFail = "localhost 403 " + nickname + " " + channelName + " :No such channel\r\n";
		send(clientSocket, kickFail.c_str(), kickFail.size(), 0);
		return ;
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
		std::cout << (itClient->second)->getIsInvited(channelName) << std::endl;
		if ((itChannel->second)->getInviteOnly() && !(itClient->second)->getIsInvited(channelName)) {
			std::string joinFail = ":localhost 473 " + nickname + " " + channelName + " :Cannot join channel (+i)\r\n";
			send(clientSocket, joinFail.c_str(), joinFail.size(), 0);
			return ;
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
}

void	handleInviteCommand(int clientSocket, const std::string& channelName, const std::string& guest, clientMap& clients, channelMap& channels) {
	// Check if the person kicking has operator privilege
	std::string			nickname;
	clientMap::iterator	it = clients.begin();
	while (it != clients.end()) {
		if ((it->second)->getSocket() == clientSocket) {
			if ((it->second)->getOperator(channelName) == false) {
				std::string	inviteFail = "localhost 482 " + nickname + " " + channelName + " :You're not on channel operator\r\n";
				send(clientSocket, inviteFail.c_str(), inviteFail.size(), 0);
				return ;
			}
			nickname = (it->second)->getNickname();
			break;
		}
		++it;
	}
	if (it == clients.end()) {
		std::string	kickFail = "localhost 442 " + nickname + " " + channelName + " :You're not on that channel\r\n";
		send(clientSocket, kickFail.c_str(), kickFail.size(), 0);
		return ;
	}
	// Check if the channel exists
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		std::string	inviteFail = "localhost 403 " + nickname + " " + channelName + " :No such channel\r\n";
		send(clientSocket, inviteFail.c_str(), inviteFail.size(), 0);
		return ;
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
		std::string	inviteFail = "localhost 442 " + guest + " " + channelName + " :Client just doesn't exist\r\n";
		send(clientSocket, inviteFail.c_str(), inviteFail.size(), 0);
		return ;
	}
	// Check if the user is in the channel
	std::vector<int>& members = (itChannel->second)->getClients();
	std::vector<int>::iterator pos = std::find(members.begin(), members.end(), userSocket);
	if (pos != members.end()) {
		std::string	inviteFail = "localhost 443 " + nickname + " " + guest + " " + channelName + ":is already on channel\r\n";
		send(clientSocket, inviteFail.c_str(), inviteFail.size(), 0);
		return ;
	}
	std::string	inviteMessage = ":localhost 341 " + nickname + " " + guest + " " + channelName + "\r\n";
	send(clientSocket, inviteMessage.c_str(), inviteMessage.length(), 0);
	inviteMessage = ":" + nickname + "!~user@host INVITE :" + channelName + " " + guest + "\r\n";
	send(userSocket, inviteMessage.c_str(), inviteMessage.length(), 0);
	std::cout << "The client got invited" << std::endl;
	if ((itChannel->second)->getInviteOnly() == true) {
		std::cout << "We just set the invite to true" << std::endl;
		(it->second)->setInvite(channelName, true);
	}
}
