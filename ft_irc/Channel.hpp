/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 17:49:07 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 16:39:13 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "Client.hpp"

class Client;

typedef std::map<std::string, Client *> clientMap;

class Channel {

public:
	
	Channel(std::string channelName);
	~Channel();

	const std::string&	getChannelName() const;
	const std::string&	getTopic() const;
	std::vector<int>&	getClients();
	const std::string&	getKey() const;
	int&				getUsers();
	const int&			getUserLimit() const;
	const bool&			getInviteOnly() const;
	const bool&			getTopicControl() const;

	void	setTopic(const std::string& topic);
	void	setInviteOnly(bool status);
	void	setTopicControl(bool status);
	void	setKey(const std::string& newKey);
	void	setUserLimit(int limit);

	void	addClient(const int& clientSocket);
	void	removeKey();
	void	removeUserLimit();

private:

	std::string			_channelName;
	std::vector<int>	_clients;
	std::string 		_topic;
	bool				_inviteOnly;
	bool				_topicControl;
	std::string			_key;
	int					_users;
	int					_userLimit;
};
