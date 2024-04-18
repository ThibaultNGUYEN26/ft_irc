/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 15:30:01 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/18 16:12:15 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <map>

#include "Channel.hpp"

class Channel;

class Client {

public:

	Client(int &socketClient, std::string &username, std::string &nickname);
	~Client();

	void	setUsername(std::string &username);
	void	setNickname(std::string &nickname);
	const std::string	&getUsername() const;
	const std::string	&getNickname() const;
	const int			&getSocket() const;
	const bool			&getOperator() const;
	void				setOperator(bool status);
	void		addChannel(Channel *newChannel, const std::string& channelName);

private:

	int			_socketClient;
	std::string	_username;
	std::string	_nickname;
	bool		_isOperator;
	std::map<std::string, Channel *>	_channels;
};
