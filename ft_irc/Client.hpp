/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 15:30:01 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/17 17:38:06 by rchbouki         ###   ########.fr       */
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

	void	setUsername(std::string &username);
	void	setNickname(std::string &nickname);
	const std::string	&getUsername() const;
	const std::string	&getNickname() const;
	const int			&getSocket() const;
	void		addChannel(Channel *newChannel, const std::string& channelName);

private:

	int			_socketClient;
	std::string	_username;
	std::string	_nickname;
	std::map<std::string, Channel *>	_channels;

};
