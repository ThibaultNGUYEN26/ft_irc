/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 17:49:07 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/17 11:58:56 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

class Channel {

public:
	
	Channel(std::string channelName);
	~Channel();

	const std::string& getChannelName() const;
	const std::string& getTopic() const;

	void setTopic(const std::string& topic);

private:

	std::string	_channelName;
	std::string _topic;

};
