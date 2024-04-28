/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 18:03:13 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/28 20:19:08 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

int getUserSocket(const std::string& nickname, clientMap& clients) {
	for (clientMap::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			return it->second->getSocket();
		}
	}
	return -1;
}

// Modify or check channel modes
void Ircserv::handleModeCommand(int clientSocket, const std::string& channelName, const std::string& modeSequence, const std::string& parameter, clientMap& clients, channelMap& channels) {
	if (channelName[0] != '#') {
		return ;
	}
	// Nickname of the client
	std::string nickname;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			if ((it->second)->getOperator(channelName) == false) {
				return ERRNOTOPERATOR(nickname, channelName, clientSocket);
			}
			nickname = (it->second)->getNickname();
			break;
		}
	}
	// Look for the channel
	channelMap::iterator channelIt = channels.find(channelName);
	if (channelIt == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	Channel* channel = channelIt->second;
	std::vector<int>& members = (channelIt->second)->getClients();
	std::vector<int>::iterator pos = std::find(members.begin(), members.end(), clientSocket);
	if (pos == members.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	int targetSocket = 0;
	bool status = true;
	for (size_t i = 0; i < modeSequence.size(); ++i) {
		char mode = modeSequence[i];
		if (mode == '+') {
			status = true;
		} else if (mode == '-') {
			status = false;
		} else {
			switch (mode) {
				case 'i':
					channel->setInviteOnly(status);
					break;
				case 't':
					channel->setTopicControl(status);
					break;
				case 'k':
					if (status) {
						channel->setKey(parameter);
					} else {
						channel->removeKey();
					}
					break;
				case 'o':
					targetSocket = getUserSocket(parameter, clients);
					if (targetSocket != -1) {
						channel->setOperator(clientSocket, targetSocket, status, clients, channelName);
					}
					break;
				case 'l':
					if (status) {
						int limit = atoi(parameter.c_str());
						channel->setUserLimit(limit);
					} else {
						channel->removeUserLimit();
					}
					break;
				default:
					std::cerr << "Unknown mode: " << mode << std::endl;
			}
		}
	}
	// Broadcast mode change to all channel members
	std::string modeChangeMessage = ":" + nickname + " MODE " + channelName + " " + modeSequence + " " + parameter + "\r\n";
	broadcastToChannel(clientSocket, channelName, modeChangeMessage, clients, channels);
}

bool	isValidNickname(const std::string& nickname, clientMap& clients, int& clientSocket) {
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

void	handleLeaveCommand(int clientSocket, const std::string& channelName, clientMap& clients, channelMap& channels) {
	// Check if the channel exists
	std::string					nickname;
	std::vector<int>			*members;
	std::vector<int>::iterator	memberIt;
	clientMap::iterator			itClient;
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel != channels.end()) {
		// Find the client's socket in the channel members list and remove it
		members = &((channels[channelName])->getClients());
		memberIt = std::find(members->begin(), members->end(), clientSocket);
		if (memberIt == members->end()) {
			return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
		}
		itClient = clients.begin();
		while (itClient != clients.end()) {
			if ((itClient->second)->getSocket() == clientSocket) {
				nickname = (itClient->second)->getNickname();
				(itClient->second)->setOperator(channelName, false);
				break;
			}
			++itClient;
		}
		members->erase(memberIt);
		// Take off the invite flag for the client leaving
		if ((itChannel->second)->getInviteOnly() == true) {
			(itClient->second)->setInvite(channelName, false);
		}
		// If the channel becomes empty, remove it from the channels map
		if (members->empty()) {
			channels.erase(itChannel);
		}
	}
	else {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
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

void handleKickCommand(int clientSocket, const std::string& channelName, const std::string& userToKick, const std::string& reason, clientMap& clients, channelMap& channels) {
	// Check if the person kicking has operator privilege
	std::string nickname;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			if ((it->second)->getOperator(channelName) == false) {
				return ERRNOTONCHANNEL(nickname, channelName, clientSocket);
			}
			nickname = (it->second)->getNickname();
			break;
		}
	}
	// Check if the channel exists
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	// Find the user to kick based on their nickname
	int userSocket;
	clientMap::iterator it = clients.begin();
	while (it != clients.end()) {
		if (it->second->getNickname() == userToKick) {
			userSocket = it->second->getSocket();
			break;
		}
		++it;
	}
	if (it == clients.end()) {
		return ERRCLIENTUNKNOWN(userToKick, channelName, clientSocket);
	}
	// Check if the user is in the channel
	std::vector<int>& members = (itChannel->second)->getClients();
	std::vector<int>::iterator pos = std::find(members.begin(), members.end(), userSocket);
	if (pos == members.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	// Set operator privilege to false for person getting kicked out
	(it->second)->setOperator(channelName, false);
	// Notify the kicked user and all channel members
	std::string kickMessage = ":" + nickname + "!~user@host KICK " + channelName + " " + userToKick + " :" + (reason.empty() ? "No reason" : reason) + "\r\n";
	// Send message to all clients in the channel and the kicked user
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
		(it->second)->setInvite(channelName, false);
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
	channelMap::iterator itChannel = channels.find(channelName);
	if (itChannel == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, senderSocket);
	}
	// Construct the message using the sender's nickname.
	std::string fullMessage = ":" + nickname + "!~user@host PRIVMSG " + channelName + " :" + message + "\r\n";
	// Broadcast the message to all channel members except the sender.
	std::vector<int>& members = (itChannel->second)->getClients();
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != senderSocket) {
			send(*memberIt, fullMessage.c_str(), fullMessage.length(), 0);
		}
	}
}

void	sendDM(int senderSocket, const std::string& target, const std::string& message, clientMap& clients) {
	// Check if the target exists
	int	targetSocket;
 	std::map<std::string, Client*>::iterator it = clients.find(target);
	if (it == clients.end()) {
		std::string fullMessage = target + "  :Client just doesn't exist\r\n";
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
	// Get nickname of the client
	std::string nickname;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			if ((it->second)->getOperator(channelName) == false) {
				return ERRNOTOPERATOR(nickname, channelName, clientSocket);
			}
			nickname = (it->second)->getNickname();
			break;
		}
	}
	// Check if the channel exists
	channelMap::iterator channelIt = channels.find(channelName);
	if (channelIt == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
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
			return ERRNOTONCHANNEL(nickname, channelName, clientSocket);
		}
		std::string topicMessage = ":" + nickname + "!~user@host TOPIC " + channelName + " :" + newTopic + "\r\n";
		for (std::vector<int>::iterator it = members.begin(); it != members.end(); it++) {
			send(*it, topicMessage.c_str(), topicMessage.length(), 0);
		}
		(channelIt->second)->setTopic(newTopic);
	} else {
		const std::string&	topic = (channelIt->second)->getTopic();
		if (topic.empty()) {
			RPL_NOTOPIC(nickname, channelName, clientSocket);
		} else {
			RPL_TOPIC(nickname, channelName, topic, clientSocket);
		}
	}
}
