/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Register.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:28:16 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/28 21:30:30 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

bool	registerClient(int& clientSocket, const std::string& _password, clientMap& clients) {
	bool		gotEnd = false, gotPassword = false;
	std::string	receivedNickname;
	std::string	receivedUsername;

	while (!gotEnd)
	{
		char buffer[1024];
		
		memset(buffer, 0, sizeof(buffer));
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead <= 0) { 
			std::cout << "Failed to receive data or connection closed." << std::endl;
			return false;
		}
		buffer[bytesRead] = '\0';

		std::string receivedCommand(buffer);
		std::istringstream iss(receivedCommand);
		std::string	cmd;
		std::cout << buffer << std::endl;
		while (std::getline(iss, cmd, ' ')) {
			if (cmd == "CAP") {
				std::string	capParam;
				std::getline(iss, capParam, '\r');
				if (capParam == "END") {
					gotEnd = true;
					break;
				}
				else {
					std::string ack;
					if (capParam[2] != '\0') {
						std::string	token;
						std::getline(iss, token, '\r');
						ack = "CAP * " + capParam + " " + token + "\r\n";
					}
					else {
						ack = "CAP * " + capParam + "\r\n";
					}
					send(clientSocket, ack.c_str(), ack.length(), 0);
				}
			}
			else if (cmd == "PASS") {
				std::string	receivedPassword;
				std::getline(iss, receivedPassword, '\r');
				if (receivedPassword != _password) {
					ERRINCORRECTPASSWORD(clientSocket);
					return false;
				}
				gotPassword = true;
			}
			else if (cmd == "NICK") {
				std::getline(iss, receivedNickname, '\r');
				if (!isValidNickname(receivedNickname, clients, clientSocket)) {
					return false;
				}
			}
			else if (cmd == "USER") {
				std::getline(iss, receivedUsername, ' ');
			}
			std::getline(iss, cmd, '\n');
		}
	}
	if (!gotPassword) {
		return false;
	}
	WELCOME_001(receivedNickname, clientSocket);
	clients[receivedNickname] = new Client(clientSocket, receivedUsername, receivedNickname);
	std::cout << GREEN "Client : {" << receivedNickname << ", " << receivedUsername << ", " << clientSocket << "} successfully connected." EOC << std::endl;
	return true;
}
