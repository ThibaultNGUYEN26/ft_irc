/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:19:47 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/01 17:09:46 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void	executeJoinCommand(int clientSocket, const std::string& channelName, const std::string& key, clientMap& clients, channelMap& channels) {
	// Get client nickname
	clientMap::iterator	itClient = getClientIterator(clientSocket, clients);
	std::string	nickname = (itClient->second)->getNickname();
	// Check if channel format is correct
	if (channelName.empty()) {
		return ERRMOREPARAMS(clientSocket, nickname, "JOIN");
	}
	if (channelName[0] != '#') {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	// Check if the channel exists, create it if not
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		channels[channelName] = new Channel(channelName);
		(channels[channelName])->addClient(clientSocket);
		(itClient->second)->addChannel(channelName, true);
		channelMap::iterator itChannel = channels.find(channelName);
		std::cout << "current users : " << (itChannel->second)->getUsers() << std::endl;
	}
	else {
		if (!((itChannel->second)->getKey().empty()) && key != (itChannel->second)->getKey()) {
			return ERRINCORRECTKEY(nickname, channelName, clientSocket);
		}
		std::cout << "limit of channel : " << (itChannel->second)->getUserLimit() << " and current users : " << (itChannel->second)->getUsers() << std::endl;
		if ((itChannel->second)->getUserLimit() != -1 && (itChannel->second)->getUserLimit() <= (itChannel->second)->getUsers()) {
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

void	handleJoinCommand(std::string& command, clientMap& clients, channelMap& channels, int& clientSocket) {
	// Extract the channel name from the command
	command = command.substr(5, command.length());
	std::istringstream iss(command);
	std::string	channelsToJoin, keys, channelName, key;
	std::getline(iss, channelsToJoin, ' ');
	if (channelsToJoin.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "JOIN");
	}
	std::getline(iss, keys, '\r');
	std::cout << keys << std::endl;
	std::istringstream ssChannel(channelsToJoin), ssKey(keys);
	while (std::getline(ssChannel, channelName, ',')) {
		if (channelName[channelName.length() - 1] == '\n') {
			if (channelName[channelName.length() - 2] == '\r') {
				channelName.erase(channelName.length() - 2, 2);;
			}
			else {
				channelName.erase(channelName.length() - 1);;
			}
		}
		else {
			std::getline(ssKey, key, ',');
		}
		std::cout << "Name *" << channelName << "*, key *" << key << "*\n";
		if (channelName.empty()) {
			return ERRMOREPARAMS(clientSocket, "", "JOIN");
		}
		executeJoinCommand(clientSocket, channelName, key, clients, channels);
	}
}
