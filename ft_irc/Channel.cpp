/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 17:52:17 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/18 16:05:46 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

Channel::Channel(std::string channelName) : _channelName(channelName) {}

Channel::~Channel() {}

const	std::string& Channel::getChannelName() const {
	return _channelName;
}

const	std::string& Channel::getTopic() const {
	return _topic;
}

std::vector<int>&	Channel::getClients() {
	return _clients;
}

void	Channel::addClient(const int& clientSocket) {
	_clients.push_back(clientSocket);
}

void	Channel::setTopic(const std::string& topic) {
	_topic = topic;
}

void	Channel::setOperator(int clientSocket, int targetSocket, bool status, clientMap clients) {
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if (clientSocket == (it->second)->getSocket()) {
			if ((it->second)->getOperator() == false)
				return ;
			if (clientSocket == targetSocket) {
				(it->second)->setOperator(status);
			}
			else {
				for (clientMap::iterator itTarget = clients.begin(); itTarget != clients.end(); itTarget++) {
					if (targetSocket == (itTarget->second)->getSocket()) {
						(itTarget->second)->setOperator(status);
					}
				}
			}
			break;
		}
	}
}

void	Channel::setInviteOnly(bool status) {
	_inviteOnly = status;
}

void	Channel::setTopicControl(bool status) {
	_topicControl = status;
}

void	Channel::setKey(const std::string& newKey) {
	_key = newKey;
}

void	Channel::removeKey() {
	_key.clear();
}

void	Channel::setUserLimit(int limit) {
	_userLimit = limit;
}

void	Channel::removeUserLimit() {
	_userLimit = -1;
}
