/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 17:52:17 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/28 20:02:39 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

Channel::Channel(std::string channelName) : _channelName(channelName), _topic(""), _inviteOnly(false), _key(""), _userLimit(-1) {
}

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

const	std::string& Channel::getKey() const {
	return _key;
}

const int&	Channel::getUsers() const {
	return _users;
}

const int&	Channel::getUserLimit() const {
	return _userLimit;
}

const bool&		Channel::getInviteOnly() const {
	return _inviteOnly;
}

void	Channel::addClient(const int& clientSocket) {
	_clients.push_back(clientSocket);
	_users++;
}

void	Channel::setTopic(const std::string& topic) {
	_topic = topic;
}

void	Channel::setOperator(int clientSocket, int targetSocket, bool status, clientMap clients, const std::string& channelName) {
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if (clientSocket == (it->second)->getSocket()) {
			if ((it->second)->getOperator(channelName) == false)
				return ;
			if (clientSocket == targetSocket) {
				(it->second)->setOperator(channelName, status);
			}
			else {
				for (clientMap::iterator itTarget = clients.begin(); itTarget != clients.end(); itTarget++) {
					if (targetSocket == (itTarget->second)->getSocket()) {
						(itTarget->second)->setOperator(channelName, status);
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

void	Channel::setUserLimit(int limit) {
	if (limit < 0) {
		return ;
	}
	_userLimit = limit;
}

void	Channel::removeKey() {
	_key.clear();
}

void	Channel::removeUserLimit() {
	_userLimit = -1;
}
