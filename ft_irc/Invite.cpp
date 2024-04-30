/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:21:00 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/30 15:40:14 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void	executeInviteCommand(int clientSocket, const std::string& channelName, const std::string& guest, clientMap& clients, channelMap& channels) {
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
