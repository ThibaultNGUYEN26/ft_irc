/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 15:30:05 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/02 19:34:16 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int &socketClient, std::string &username, std::string &nickname) : _socketClient(socketClient), _username(username), _nickname(nickname) {}

Client::~Client() {
	close(_socketClient);
}

void	Client::setUsername(std::string &username) {
	_username = username;
}

void	Client::setNickname(std::string &nickname) {
	_nickname = nickname;
}

const std::string &Client::getUsername() const {
	return _username;
}

const std::string &Client::getNickname() const {
	return _nickname;
}

const int	&Client::getSocket() const {
	return _socketClient;
}

bool	Client::getIsInvited(const std::string& channelName) const {
	std::map<std::string, bool>::const_iterator	it = _channelInvite.find(channelName);
	if (it != _channelInvite.end()) {
		return it->second;
	}
	return false;
}

bool	Client::getOperator(const std::string& channelName) const {
	std::map<std::string, bool>::const_iterator	it = _channelOperator.find(channelName);
	if (it != _channelOperator.end()) {
		return it->second;
	}
	return false;
}

void	Client::setInvite(const std::string& channelName, bool status) {
	_channelInvite[channelName] = status;
}

void	Client::setOperator(const std::string& channelName, bool status) {
	_channelOperator[channelName] = status;
}

void	Client::addChannel(const std::string& channelName, const bool& status) {
	_channelOperator[channelName] = status;
}
