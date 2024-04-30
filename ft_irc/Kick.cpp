/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:23:25 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/30 15:23:16 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void	executeKickCommand(int clientSocket, const std::string& channelName, const std::string& userToKick, const std::string& reason, clientMap& clients, channelMap& channels) {
	// Check if the person kicking has operator privilege
	clientMap::iterator	itClient = getClientIterator(clientSocket, clients);
	std::string	nickname = (itClient->second)->getNickname();
	if (channelName.empty()) {
		return ERRMOREPARAMS(clientSocket, nickname, "KICK");
	}
	if ((itClient->second)->getOperator(channelName) == false) {
		return ERRNOTOPERATOR(nickname, channelName, clientSocket);
	}
	// Check if the channel exists
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	// Find the user to kick based on their nickname
	int userSocket = getUserSocket(userToKick, clients);
	if (userSocket == -1) {
		return ERRCLIENTUNKNOWN(userToKick, channelName, clientSocket);
	}
	// Check if the user is in the channel
	std::vector<int>& members = (itChannel->second)->getClients();
	std::vector<int>::iterator pos = std::find(members.begin(), members.end(), userSocket);
	if (pos == members.end()) {
		return ERRNOTINCHANNEL(nickname, userToKick, channelName, clientSocket);
	}
	// Set operator privilege to false for person getting kicked out
	(itClient->second)->setOperator(channelName, false);
	// Notify the kicked user and all channel members
	std::string kickMessage = ":" + nickname + "!~user@host KICK " + channelName + " " + userToKick + " :" + (reason.empty() ? "No reason" : reason) + "\r\n";
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != userSocket) {
			send(*memberIt, kickMessage.c_str(), kickMessage.length(), 0);
		}
	}
	kickMessage = ":" + userToKick + "!~user@host PART :" + channelName + "\r\n";
	send(userSocket, kickMessage.c_str(), kickMessage.length(), 0);
	// Remove the user from the channel
	members.erase(pos);
	if ((itChannel->second)->getInviteOnly() == true) {
		(itClient->second)->setInvite(channelName, false);
	}
}

void	handleKickCommand(std::string& command, clientMap& clients, channelMap& channels, int& clientSocket) {
	command = command.substr(5, command.length());
	std::istringstream iss(command);
	std::string channelName, userToKick, reason;
	std::getline(iss, channelName, ' ');
	std::cout << "Name *" << channelName << "*\n";
	if (checkNC(channelName) || channelName.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "KICK");
	}
	std::getline(iss, userToKick, ' ');
	std::cout << "userToKick *" << userToKick << std::endl;
	if (userToKick.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "KICK");
	}
	if (!checkNC(userToKick)) {
		std::getline(iss, reason, ':');
		std::getline(iss, reason, '\r');
		checkNC(reason);
		// Trim any leading spaces from the reason
		size_t startPos = reason.find_first_not_of(" ");
		if (startPos != std::string::npos) {
			reason = reason.substr(startPos);
		}
		std::cout << "Reason *" << reason << "*\n";
	}
	executeKickCommand(clientSocket, channelName, userToKick, reason, clients, channels);
}
