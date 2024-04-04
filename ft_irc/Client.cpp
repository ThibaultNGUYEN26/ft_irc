/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 15:30:05 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/03 15:50:14 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int socketClient) : _socketClient(socketClient), _active(true) {}

Client::~Client() {
    close(_socketClient);
}

bool Client::isActive() const {
    return _active;
}

void Client::handleActivity() {
    if (_active) {
        readMessage();
        // If there were other activities (e.g., writing), they'd be handled here.
    }
}

void Client::readMessage() {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = read(_socketClient, buffer, sizeof(buffer) - 1);

    if (bytesRead > 0) {
        processMessage(std::string(buffer, bytesRead));
    } else {
        _active = false;
    }
}

void Client::sendMessage(const std::string& message) {
    write(_socketClient, message.c_str(), message.length());
}

void Client::processMessage(const std::string& message) {
    std::cout << "Message from client: " << message << std::endl;
    // Echo back the message as a simple response
    sendMessage("Echo: " + message);
}