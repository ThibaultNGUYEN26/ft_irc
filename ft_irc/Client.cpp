/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 15:30:05 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/17 17:38:01 by rchbouki         ###   ########.fr       */
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

void	Client::addChannel(Channel *newChannel, const std::string& channelName) {
	_channels[channelName] = newChannel;
}
