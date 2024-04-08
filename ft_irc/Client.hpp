/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 15:30:01 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/08 15:10:11 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <map>

#include "Channel.hpp"

class Client {

public:

	Client(int &socketClient, std::string &username, std::string &nickname);
	~Client();

	/* bool isActive() const;
	void handleActivity(); */
	void	setUsername(std::string &username);
	void	setNickname(std::string &nickname);
	const std::string &getUsername() const;
	const std::string &getNickname() const;
	const int	&getSocket() const;
	//const std::string	getChannel()

	void	addChannel(Channel &newChannel);

private:

	int			_socketClient;
	std::string	_username;
	std::string	_nickname;
	std::map<std::string, Channel>	_channels;


	/* void readMessage();
	void sendMessage(const std::string& message);
	void processMessage(const std::string& message); */

};
