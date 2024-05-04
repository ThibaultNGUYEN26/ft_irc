/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Leave.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:22:46 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 19:54:37 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void	executeLeaveCommand(int clientSocket, const std::string& channelName, clientMap& clients, channelMap& channels) {
	// Check if the channel exists
	clientMap::iterator	itClient = getClientIterator(clientSocket, clients);
	std::string	nickname = (itClient->second)->getNickname();
	std::string	username = (itClient->second)->getUsername();
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	else {
		// Find the client's socket in the channel members list and remove it
		std::vector<int>	*members = &((channels[channelName])->getClients());
		std::vector<int>::iterator	memberIt = std::find(members->begin(), members->end(), clientSocket);
		if (memberIt == members->end()) {
			return ERRNOTONCHANNEL(nickname, channelName, clientSocket);
		}
		members->erase(memberIt);
		// Remove the invite flag and the operator privileges for the client leaving
		if ((itChannel->second)->getInviteOnly() == true) {
			(itClient->second)->setInvite(channelName, false);
		}
		(itClient->second)->setOperator(channelName, false);
		// If the channel becomes empty, remove it from the channels map
		if (members->empty()) {
			channels.erase(itChannel);
		} else {
			(itChannel->second)->getUsers() -= 1;
		}
		// Send PART message back to the client to confirm leaving
		std::string leaveConfirm = ":" + nickname + "!~" + username + "@" + std::string(HOSTNAME) + " PART :" + channelName + "\r\n";
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
	if (channelName.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "PART");
	}
	executeLeaveCommand(clientSocket, channelName, clients, channels);
}
