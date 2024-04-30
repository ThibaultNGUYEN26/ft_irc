/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Leave.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:22:46 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/30 15:49:09 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void	executeLeaveCommand(int clientSocket, const std::string& channelName, clientMap& clients, channelMap& channels) {
	// Check if the channel exists
	clientMap::iterator	itClient = getClientIterator(clientSocket, clients);
	std::string	nickname = (itClient->second)->getNickname();
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	else {
		std::vector<int>	*members = &((channels[channelName])->getClients());
		std::vector<int>::iterator	memberIt = std::find(members->begin(), members->end(), clientSocket);
		// Find the client's socket in the channel members list and remove it
		if (memberIt == members->end()) {
			return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
		}
		(itClient->second)->setOperator(channelName, false);
		members->erase(memberIt);
		// Take off the invite flag for the client leaving
		if ((itChannel->second)->getInviteOnly() == true) {
			(itClient->second)->setInvite(channelName, false);
		}
		// If the channel becomes empty, remove it from the channels map
		if (members->empty()) {
			channels.erase(itChannel);
		}
		// Send PART message back to the client to confirm leaving
		std::string leaveConfirm = ":" + nickname + "!~user@host PART :" + channelName + "\r\n";
		send(clientSocket, leaveConfirm.c_str(), leaveConfirm.size(), 0);
		// Notify all clients in the channel about the member leaving
		if (!members->empty()) {
			memberIt = members->begin();
			while (memberIt != members->end()) {
				send(*memberIt, leaveConfirm.c_str(), leaveConfirm.size(), 0);
				++memberIt;
			}
		}
	}
}

void	handleLeaveCommand(std::string& command, clientMap& clients, channelMap& channels, int& clientSocket) {
	command = command.substr(5, command.length());
	std::istringstream iss(command);
	std::string channelName;
	std::getline(iss, channelName, '\r');
	checkNC(channelName);
	std::cout << "channelName *" << channelName << "*\n";
	if (channelName.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "PART");
	}
	executeLeaveCommand(clientSocket, channelName, clients, channels);
}
