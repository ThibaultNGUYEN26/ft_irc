/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 18:03:13 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/11 14:11:22 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

bool	isValidNickname(const std::string& nickname, clientMap& clients) {
	if (nickname.length() > 9) {
		std::cout << RED "Nickname's length exceeds 9 characters. Change it before registering again." EOC << std::endl;	
		return false;
	}
	if (clients.find(nickname) != clients.end()) {
		std::cout << RED "Nickname already exists within the server's clients. Change it before registering again." EOC << std::endl;
		return false;
	}
	return true;
}

void	handleJoinCommand(int clientSocket, const std::string& channelName, clientMap& clients, channelMap& channels) {
	// Check if the channel exists, create it if not
	std::map<std::string, std::vector<int> >::iterator it = channels.find(channelName);
	if (it == channels.end()) {
		std::vector<int> clientSockets;
		clientSockets.push_back(clientSocket);
		channels[channelName] = clientSockets;
	} else {
		it->second.push_back(clientSocket);
	}
	// Notify all clients in the channel about the new member
	std::string nickname;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			nickname = (it->second)->getNickname();
			break;
		}
	}
	// Send JOIN message back to the client to confirm
	std::string joinConfirm = ":" + nickname + "!~user@host JOIN :" + channelName + "\r\n";
	send(clientSocket, joinConfirm.c_str(), joinConfirm.size(), 0);

	std::vector<int>& members = channels[channelName];
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != clientSocket) {
			send(*memberIt, joinConfirm.c_str(), joinConfirm.size(), 0);
		}
	}
}
	
void	broadcastToChannel(int senderSocket, const std::string& channelName, const std::string& message, clientMap& clients, channelMap& channels) {
	// Iterate over clients to find the nickname of the sender.
	std::string nickname;
	for (std::map<std::string, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->getSocket() == senderSocket) {
			nickname = it->second->getNickname();
			break;
		}
	}
	// Find the channel in the map
	std::map<std::string, std::vector<int> >::iterator channelIt = channels.find(channelName);
	if (channelIt == channels.end()) {
		std::string fullMessage = nickname + " " + channelName + " :" + "No such channel" + "\r\n";
		send(senderSocket, fullMessage.c_str(), fullMessage.length(), 0);
		return ;
	}
	// Construct the message using the sender's nickname.
	std::string fullMessage = ":" + nickname + "!~user@host PRIVMSG " + channelName + " :" + message + "\r\n";

	// Broadcast the message to all channel members except the sender.
	std::vector<int>& members = channelIt->second;
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != senderSocket) {
			send(*memberIt, fullMessage.c_str(), fullMessage.length(), 0);
		}
	}
}

void	sendDM(int senderSocket, const std::string& target, const std::string& message, clientMap& clients) {
	// Check if the target exists
 	std::map<std::string, Client*>::iterator it = clients.find(target);
	int	targetSocket;
	if (it == clients.end()) {
		std::string fullMessage = target + " : There was no such channel\r\n";
		send(senderSocket, fullMessage.c_str(), fullMessage.length(), 0);
		return ;
	}
	else {
		targetSocket = clients[target]->getSocket();
	}
	// retrieve nickname of the sender
	std::string nickname;
	for (std::map<std::string, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->getSocket() == senderSocket) {
			nickname = it->second->getNickname();
			break;
		}
	}
	std::string fullMessage = ":" + nickname + " PRIVMSG " + target + " :" + message + "\r\n";
	send(targetSocket, fullMessage.c_str(), fullMessage.length(), 0);
}
