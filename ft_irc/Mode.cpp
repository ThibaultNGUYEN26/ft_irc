/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:21:51 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 19:54:51 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void executeModeCommand(int clientSocket, const std::string& channelName, const std::string& modeSequence, const std::string& parameter, clientMap& clients, channelMap& channels) {
	// Check if MODE is well for channels
	if (channelName[0] != '#') {
		return ;
	}
	// Nickname of the client
	clientMap::iterator	itClient = getClientIterator(clientSocket, clients);
	std::string	nickname = (itClient->second)->getNickname();
	std::string	username = (itClient->second)->getUsername();
	// Look for the channel
	channelMap::iterator channelIt = channels.find(channelName);
	if (channelIt == channels.end()) {
		return ERRNOSUCHCHANNEL(nickname, channelName, clientSocket);
	}
	// Check if the client is an operator
	if ((itClient->second)->getOperator(channelName) == false) {
		return ERRNOTOPERATOR(nickname, channelName, clientSocket);
	}
	std::vector<int>& members = (channelIt->second)->getClients();
	std::vector<int>::iterator pos = std::find(members.begin(), members.end(), clientSocket);
	if (pos == members.end()) {
		return ERRNOTONCHANNEL(nickname, channelName, clientSocket);
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
						if (parameter.empty()) {
							return ERRMOREPARAMS(clientSocket, "", "MODE");
						}
						(channelIt->second)->setKey(parameter);
					} else {
						(channelIt->second)->removeKey();
					}
					break;
				case 'o':
					if (parameter.empty()) {
						return ERRMOREPARAMS(clientSocket, "", "MODE");
					}
					targetSocket = getUserSocket(parameter, clients);
					if (targetSocket != -1) {
						(getClientIterator(targetSocket, clients)->second)->setOperator(channelName, status);
					}
					else {
						return ERRCLIENTUNKNOWN(parameter, channelName, clientSocket);
					}
					break;
				case 'l':
					if (status) {
						if (parameter.empty()) {
							return ERRMOREPARAMS(clientSocket, "", "MODE");
						}
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
	std::string modeChangeMessage = ":" + nickname + "!~" + username + "@" + std::string(HOSTNAME) + " PRIVMSG " + channelName + " :" + ":" + nickname + " MODE " + channelName + " " + modeSequence + " " + parameter + "\r\n";
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != clientSocket) {
			send(*memberIt, modeChangeMessage.c_str(), modeChangeMessage.length(), 0);
		}
	}
}

void	handleModeCommand(std::string& command, clientMap& clients, channelMap& channels, int& clientSocket) {
	command = command.substr(5, command.length());
	std::istringstream iss(command);
	std::string channelName, modeCommand;
	std::string	param = "";
	std::getline(iss, channelName, ' ');
	checkNC(channelName);
	if (channelName.empty()) {
		return ERRMOREPARAMS(clientSocket, "", "MODE");
	}
	std::getline(iss, modeCommand, ' ');
	if (!checkNC(modeCommand) && modeCommand[modeCommand.length() - 1] != '\r') {
		std::getline(iss, param, '\r');
		checkNC(param);
	}
	else if (modeCommand[modeCommand.length() - 1] == '\r') {
		modeCommand.erase(modeCommand.length() - 1);
	}
	executeModeCommand(clientSocket, channelName, modeCommand, param, clients, channels);
}
