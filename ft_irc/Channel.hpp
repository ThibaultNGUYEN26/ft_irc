/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 17:49:07 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/17 19:07:02 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>

class Channel {

public:
	
	Channel(std::string channelName);
	~Channel();

	const std::string&	getChannelName() const;
	const std::string&	getTopic() const;
	std::vector<int>&	getClients();

	void	setTopic(const std::string& topic);
	void	addClient(const int& clientSocket);
private:

	std::string			_channelName;
	std::vector<int>	_clients;
	std::string 		_topic;

};
