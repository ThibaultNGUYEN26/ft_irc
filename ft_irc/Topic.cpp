/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:24:38 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 19:55:49 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void executeTopicCommand(int clientSocket, const std::string& channelName, const std::string& newTopic, clientMap& clients, channelMap& channels) {
	// Get nickname of the client
	clientMap::iterator	itClient = getClientIterator(clientSocket, clients);
	std::string	nickname = (itClient->second)->getNickname();
	std::string	username = (itClient->second)->getUsername();
	if ((itClient->second)->getOperator(channelName) == false) {
		return ERRNOTOPERATOR(nickname, channelName, clientSocket);
	}
	// Check if the channel exists
	channelMap::iterator channelIt = channels.find(channelName);
	if (channelIt == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	if ((channelIt->second)->getTopicControl() == false) {
		return ;
	}
	// Check if a new topic is provided
	if (!newTopic.empty()) {
		// Check if the client sending the command is in the channel
		bool	clientInChannel = false;
		std::vector<int>&	members = (channelIt->second)->getClients();
		std::vector<int>::iterator pos = std::find(members.begin(), members.end(), clientSocket);
		if (pos != members.end()) {
			clientInChannel = true;
		}
		if (!clientInChannel) {
			return ERRNOTONCHANNEL(nickname, channelName, clientSocket);
		}
		std::string topicMessage = ":" + nickname + "!~" + username + "@" + std::string(HOSTNAME) + " TOPIC " + channelName + " :" + newTopic + "\r\n";
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

void	handleTopicCommand(std::string& command, clientMap& clients, channelMap& channels, int& clientSocket) {
	command = command.substr(6, command.length());
	std::istringstream iss(command);
	std::string channelName, topic;
	std::getline(iss, channelName, ' ');
	checkNC(channelName);
	if (channelName.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "TOPIC");
	}
	std::getline(iss, topic, ':');
	std::getline(iss, topic, '\r');
	checkNC(topic);
	if (topic.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "TOPIC");
	}
	executeTopicCommand(clientSocket, channelName, topic, clients, channels);
}
