/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 20:25:19 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/29 17:52:46 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcUtils.hpp"

bool isValidNickname(const std::string &nickname, clientMap &clients, int &clientSocket) {
	if (nickname.length() > 9) {
		std::string nickFail = ":localhost 432 " + nickname + " :Erroneus nickname\r\n";
		send(clientSocket, nickFail.c_str(), nickFail.size(), 0);
		return false;
	}
	if (clients.find(nickname) != clients.end()) {
		return false;
	}
	return true;
}

const clientMap::iterator getClientIterator(const int &clientSocket, clientMap &clients) {
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			return it;
		}
	}
	return clients.end();
}

int getUserSocket(const std::string &nickname, clientMap &clients) {
	for (clientMap::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			return it->second->getSocket();
		}
	}
	return -1;
}

void broadcastToChannel(int senderSocket, const std::string &channelName, const std::string &message, clientMap &clients, channelMap &channels) {
	// Get nickname of sender
	clientMap::iterator itClient = getClientIterator(senderSocket, clients);
	std::string nickname = (itClient->second)->getNickname();
	// Find the channel in the map
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, senderSocket);
	}
	// Construct the message using the sender's nickname.
	std::string fullMessage = ":" + nickname + "!~user@host PRIVMSG " + channelName + " :" + message + "\r\n";
	// Broadcast the message to all channel members except the sender.
	std::vector<int> &members = (itChannel->second)->getClients();
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != senderSocket) {
			send(*memberIt, fullMessage.c_str(), fullMessage.length(), 0);
		}
	}
}

void sendDM(int senderSocket, const std::string &target, const std::string &message, clientMap &clients) {
	// Check if the target exists
	int targetSocket = getUserSocket(target, clients);
	if (targetSocket == -1) {
		std::string fullMessage = target + "  :Client just doesn't exist\r\n";
		send(senderSocket, fullMessage.c_str(), fullMessage.length(), 0);
		return;
	}
	// Retrieve nickname of the sender
	clientMap::iterator itClient = getClientIterator(senderSocket, clients);
	std::string nickname = (itClient->second)->getNickname();
	std::string fullMessage = ":" + nickname + " PRIVMSG " + target + " :" + message + "\r\n";
	std::cout << fullMessage << std::endl;
	send(targetSocket, fullMessage.c_str(), fullMessage.length(), 0);
}

bool checkNC(std::string &str) {
	if (str[str.length() - 1] == '\n') {
		str.erase(str.length() - 1);
		return true;
	}
	return false;
}
