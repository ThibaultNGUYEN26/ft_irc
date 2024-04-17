/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 18:03:13 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/17 19:45:52 by rchbouki         ###   ########.fr       */
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

	channelMap::iterator	it = channels.find(channelName);
	if (it == channels.end()) {
		channels[channelName] = new Channel(channelName);
		(channels[channelName])->addClient(clientSocket);
	}
	else {
		(it->second)->addClient(clientSocket);
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

	// Inform the other clients
	std::vector<int>& members = (channels[channelName])->getClients();
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != clientSocket) {
			send(*memberIt, joinConfirm.c_str(), joinConfirm.size(), 0);
		}
	}
}

void	handleLeaveCommand(int clientSocket, const std::string& channelName, clientMap& clients, channelMap& channels) {
	// Check if the channel exists
	int	test = 0;
	std::vector<int> members;
	channelMap::iterator it = channels.find(channelName);
	
	if (it != channels.end()) {
		// Find the client's socket in the channel members list and remove it
		members = (it->second)->getClients();
		std::vector<int>::iterator memberIt = members.begin();
		int	len = ((it->second)->getClients()).size();
		while (memberIt != members.end()) {
			if (*memberIt == clientSocket) {
				memberIt = members.erase(memberIt);
				break ;
			} else {
				++memberIt;
				++test;
			}
		}
		if (test == len) {
			return ;
		}
		// If the channel becomes empty, remove it from the channels map
		if (members.empty()) {
			channels.erase(it);
		}
	}

	// Notify the client about leaving the channel
	std::string nickname;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			nickname = (it->second)->getNickname();
			break;
		}
	}

	// Send PART message back to the client to confirm leaving
	std::string leaveConfirm = ":" + nickname + "!~user@host PART :" + channelName + "\r\n";
	send(clientSocket, leaveConfirm.c_str(), leaveConfirm.size(), 0);

	// Notify all clients in the channel about the member leaving
	if (!members.empty()) {
		for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
			if (*memberIt != clientSocket) {
				send(*memberIt, leaveConfirm.c_str(), leaveConfirm.size(), 0);
			}
		}
	}
}

void handleKickCommand(int clientSocket, const std::string& channelName, const std::string& userToKick, const std::string& reason, clientMap& clients, channelMap& channels) {
	// Check if the channel exists
	channelMap::iterator channelIt = channels.find(channelName);
	if (channelIt == channels.end()) {
		std::cerr << "No such channel: " << channelName << std::endl;
		return;
	}

	// Find the user to kick based on their nickname
	int userSocket = -1;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->getNickname() == userToKick) {
			userSocket = it->second->getSocket();
			break;
		}
	}

	if (userSocket == -1) {
		std::cerr << "User not found: " << userToKick << std::endl;
		return;
	}

	// Check if the user is in the channel
	std::vector<int>& members = (channelIt->second)->getClients();
	std::vector<int>::iterator pos = std::find(members.begin(), members.end(), userSocket);
	if (pos == members.end()) {
		std::cerr << "User not found in channel: " << userToKick << std::endl;
		return;
	}

	// Remove the user from the channel
	members.erase(pos);

	std::string nickname;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			nickname = (it->second)->getNickname();
			break;
		}
	}
	// Notify the kicked user and all channel members
	std::string kickMessage = ":" + nickname + "!~user@host KICK " + channelName + " " + userToKick + " :" + (reason.empty() ? "No reason" : reason) + "\r\n";

	// Send message to all clients in the channel and the kicked user
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		send(*memberIt, kickMessage.c_str(), kickMessage.length(), 0);
	}
	send(userSocket, kickMessage.c_str(), kickMessage.length(), 0);
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
	channelMap::iterator channelIt = channels.find(channelName);
	if (channelIt == channels.end()) {
		std::string fullMessage = nickname + " " + channelName + " :" + "No such channel" + "\r\n";
		send(senderSocket, fullMessage.c_str(), fullMessage.length(), 0);
		return ;
	}
	// Construct the message using the sender's nickname.
	std::string fullMessage = ":" + nickname + "!~user@host PRIVMSG " + channelName + " :" + message + "\r\n";

	// Broadcast the message to all channel members except the sender.
	std::vector<int>& members = (channelIt->second)->getClients();
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
		std::string fullMessage = target + " : There was no such nickname\r\n";
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
	std::cout << fullMessage << std::endl;
	send(targetSocket, fullMessage.c_str(), fullMessage.length(), 0);
}

void handleTopicCommand(int clientSocket, const std::string& channelName, const std::string& newTopic, clientMap& clients, channelMap& channels) {
	// Check if the channel exists
	channelMap::iterator channelIt = channels.find(channelName);
	if (channelIt == channels.end()) {
		std::cerr << "No such channel: " << channelName << std::endl;
		return;
	}
	// Check if a new topic is provided
	if (!newTopic.empty()) {
		// Check if the client sending the command is in the channel
		bool				clientInChannel = false;
		std::vector<int>&	members = (channelIt->second)->getClients();
		for (std::vector<int>::iterator it = members.begin(); it != members.end(); it++) {
			if (*it == clientSocket) {
				clientInChannel = true;
				break;
			}
		}
		if (!clientInChannel) {
			std::cerr << "Client not in channel: " << channelName << std::endl;
			return;
		}
		std::string nickname;
		for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
			if ((it->second)->getSocket() == clientSocket) {
				nickname = (it->second)->getNickname();
				break;
			}
		}
		std::string topicMessage = ":" + nickname + "!~user@host TOPIC " + channelName + " :" + newTopic + "\r\n";
		for (std::vector<int>::iterator it = members.begin(); it != members.end(); it++) {
			send(*it, topicMessage.c_str(), topicMessage.length(), 0);
		}
		(channelIt->second)->setTopic(newTopic);
	} else {
		// If no new topic is provided, send the current topic back to the client
		if ((channelIt->second)->getTopic().empty()) {
			std::string noTopicMessage = "There is no topic set for " + channelName + "\r\n";
			send(clientSocket, noTopicMessage.c_str(), noTopicMessage.length(), 0);
		} else {
			std::string currentTopicMessage = "Current topic for " + channelName + " is: " + (channelIt->second)->getTopic() + "\r\n";
			send(clientSocket, currentTopicMessage.c_str(), currentTopicMessage.length(), 0);
		}
	}
}
