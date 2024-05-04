/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 20:25:19 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 19:53:56 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcUtils.hpp"

bool isValidNickname(const std::string &nickname, clientMap &clients, int &clientSocket) {
	if (nickname.length() > 9) {
		ERRNICKLENGTH(nickname, clientSocket);
		return false;
	}
	if (clients.find(nickname) != clients.end()) {
		ERRNICKNAMEINUSE(nickname, clientSocket);
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
	std::string username = (itClient->second)->getUsername();
	// Find the channel in the map
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, senderSocket);
	}
	// Construct the message using the sender's nickname.
	std::string fullMessage = ":" + nickname + "!~" + username + "@" + std::string(HOSTNAME) + " PRIVMSG " + channelName + " :" + message + "\r\n";
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
	if (target == "pissouBot") {
		std::string fullMessage = ":" + nickname + " PRIVMSG " + target + " :" + message + "\r\n";
		send(targetSocket, fullMessage.c_str(), fullMessage.length(), 0);
		return ;
	}
	std::string fullMessage = ":" + nickname + " PRIVMSG " + target + " :" + message + "\r\n";
	send(targetSocket, fullMessage.c_str(), fullMessage.length(), 0);
}

bool checkNC(std::string &str) {
	if (str[str.length() - 1] == '\n') {
		str.erase(str.length() - 1);
		return true;
	}
	return false;
}
