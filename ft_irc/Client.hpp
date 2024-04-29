/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 15:30:01 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/29 18:27:46 by rchbouki         ###   ########.fr       */
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
	bool				getOperator(const std::string& channelName) const;
	bool				getIsInvited(const std::string& channelName) const;
	void				setOperator(const std::string& channelName, bool status);
	void				setInvite(const std::string& channelName, bool status);
	void				addChannel(const std::string& channelName, const bool& status);

private:

	int			_socketClient;
	std::string	_username;
	std::string	_nickname;
	std::map<std::string, bool>	_channelOperator;
	std::map<std::string, bool>	_channelInvite;
};
