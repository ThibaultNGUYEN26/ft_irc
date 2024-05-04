/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 17:52:17 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 16:39:29 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

Channel::Channel(std::string channelName) : _channelName(channelName), _topic(""), _inviteOnly(false), _topicControl(true), _key(""), _users(0), _userLimit(-1) {
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

int&	Channel::getUsers() {
	return _users;
}

const int&	Channel::getUserLimit() const {
	return _userLimit;
}

const bool&		Channel::getInviteOnly() const {
	return _inviteOnly;
}

const bool&		Channel::getTopicControl() const {
	return _topicControl;
}

void	Channel::addClient(const int& clientSocket) {
	_clients.push_back(clientSocket);
	_users = _users + 1;
}

void	Channel::setTopic(const std::string& topic) {
	_topic = topic;
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
