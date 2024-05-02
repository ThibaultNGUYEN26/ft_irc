/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:21:00 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/02 19:33:29 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void	executeInviteCommand(int clientSocket, const std::string& channelName, const std::string& guest, clientMap& clients, channelMap& channels) {
	// Check if the person inviting has operator privilege
	clientMap::iterator	itClient = getClientIterator(clientSocket, clients);
	std::string	nickname = (itClient->second)->getNickname();
	std::string	username = (itClient->second)->getUsername();
	// Check if the channel exists
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	// Check if the user inviting is in the channel
	std::vector<int>& members = (itChannel->second)->getClients();
	std::vector<int>::iterator pos = std::find(members.begin(), members.end(), clientSocket);
	if (pos == members.end()) {
		return ERRNOTONCHANNEL(nickname, channelName, clientSocket);
	}
	// check if user is operator in the channel
	if ((itClient->second)->getOperator(channelName) == false) {
		return ERRNOTOPERATOR(nickname, channelName, clientSocket);
	}
	// Find the user to invite based on their nickname
	
	int userSocket = getUserSocket(guest, clients);
	if (userSocket == -1) {
		return ERRCLIENTUNKNOWN(guest, channelName, clientSocket);
	}
	// Check if the user to be invited is in the channel
	pos = std::find(members.begin(), members.end(), userSocket);
	if (pos != members.end()) {
		return ERRINCHANNEL(nickname, guest, channelName, clientSocket);
	}
	// Send inviting messages
	RPL_INVITING(nickname, guest, channelName, clientSocket);
	INVITE_MESSAGE(guest, channelName, userSocket);
	itClient = getClientIterator(userSocket, clients);
	if ((itChannel->second)->getInviteOnly() == true) {
		(itClient->second)->setInvite(channelName, true);
	}
}

void	handleInviteCommand(std::string& command, clientMap& clients, channelMap& channels, int& clientSocket) {
	command = command.substr(7, command.length());
	std::istringstream iss(command);
	std::string	nickname, channelName;
	std::getline(iss, nickname, ' ');
	if (checkNC(nickname) || nickname.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "INVITE");
	}
	std::getline(iss, channelName, '\r');
	checkNC(channelName);
	if (channelName.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "INVITE");
	}
	std::cout << "Channel: " << channelName << " nickname: " << nickname << std::endl;
	executeInviteCommand(clientSocket, channelName, nickname, clients, channels);
}
