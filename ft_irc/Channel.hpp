/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 17:49:07 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/23 20:12:08 by rchbouki         ###   ########.fr       */
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

	void	setTopic(const std::string& topic);
	void	addClient(const int& clientSocket);

	void	setOperator(int clientSocket, int targetSocket, bool status, clientMap clients);
	void	setInviteOnly(bool status);
	void	setTopicControl(bool status);
	void	setKey(const std::string& newKey);
	void	removeKey();
	void	setUserLimit(int limit);
	void	removeUserLimit();

private:

	std::string			_channelName;
	std::vector<int>	_clients;
	std::string 		_topic;

	bool				_inviteOnly;
	bool				_topicControl;
	std::string			_key;
	int					_userLimit;

};
