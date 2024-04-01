/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 15:30:01 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/01 17:34:03 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <unistd.h>
#include <cstring>
#include <iostream>

class Client {

public:

	Client(int socketFd);
	~Client();

	bool isActive() const;
	void handleActivity();

private:

	int _socketFd;
	bool _active;

	void readMessage();
	void sendMessage(const std::string& message);
	void processMessage(const std::string& message);

};
