/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:21:51 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/30 17:21:22 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void executeModeCommand(int clientSocket, const std::string& channelName, const std::string& modeSequence, const std::string& parameter, clientMap& clients, channelMap& channels) {
	if (channelName[0] != '#') {
		return ;
	}
	// Nickname of the client
	clientMap::iterator	itClient = getClientIterator(clientSocket, clients);
	std::string	nickname = (itClient->second)->getNickname();
	if ((itClient->second)->getOperator(channelName) == false) {
		return ERRNOTOPERATOR(nickname, channelName, clientSocket);
	}
	// Look for the channel
	channelMap::iterator channelIt = channels.find(channelName);
	if (channelIt == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
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
					(channelIt->second)->setInviteOnly(status);
					break;
				case 't':
					(channelIt->second)->setTopicControl(status);
					break;
				case 'k':
					if (status) {
						(channelIt->second)->setKey(parameter);
					} else {
						(channelIt->second)->removeKey();
					}
					break;
				case 'o':
					targetSocket = getUserSocket(parameter, clients);
					if (targetSocket != -1) {
						(channelIt->second)->setOperator(clientSocket, targetSocket, status, clients, channelName);
					}
					break;
				case 'l':
					if (status) {
						int limit = atoi(parameter.c_str());
						(channelIt->second)->setUserLimit(limit);
					} else {
						(channelIt->second)->removeUserLimit();
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

void	handleModeCommand(std::string& command, clientMap& clients, channelMap& channels, int& clientSocket) {
	command = command.substr(5, command.length());
	std::istringstream iss(command);
	std::string channelName, modeCommand, param;
	std::getline(iss, channelName, ' ');
	checkNC(channelName);
	if (channelName.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "MODE");
	}
	
	if (command.length() - 6 - channelName.length() != 4) {
		std::getline(iss, modeCommand, ' ');
		std::getline(iss, param, '\r');
		std::cout << "1 Mode : " << modeCommand << ", param : " << param << std::endl;
	} else {
		std::getline(iss, modeCommand, '\r');
		std::cout << "2 Mode : " << modeCommand << ", param : " << param << std::endl;
	}
	size_t startModePos = modeCommand.find_first_not_of(" ");
	size_t startParamPos = param.find_first_not_of(" ");
	if (startModePos != std::string::npos) {
		modeCommand = modeCommand.substr(startModePos);
	}
	if (startParamPos != std::string::npos) {
		param = param.substr(startParamPos);
	}
	std::cout << "Channel: " << channelName << " Mode: " << modeCommand << " Param: " << param << std::endl;
	executeModeCommand(clientSocket, channelName, modeCommand, param, clients, channels);
}
