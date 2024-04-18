/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 15:30:05 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/18 16:12:39 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int &socketClient, std::string &username, std::string &nickname) : _socketClient(socketClient), _username(username), _nickname(nickname), _isOperator(true) {}

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

void	Client::setOperator(bool status) {
	_isOperator = status;
}

const std::string &Client::getNickname() const {
	return _nickname;
}

const int	&Client::getSocket() const {
	return _socketClient;
}

const bool	&Client::getOperator() const {
	return _isOperator;
}

void	Client::addChannel(Channel *newChannel, const std::string& channelName) {
	_channels[channelName] = newChannel;
}
