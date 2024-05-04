/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Register.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 21:28:16 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 19:55:17 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

bool	registerClient(int& clientSocket, const std::string& _password, clientMap& clients) {
	bool		gotEnd = false, gotPassword = false, gotNick = false;
	std::string	receivedPassword;
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
				std::getline(iss, receivedPassword, '\r');
				checkNC(receivedPassword);
				if (receivedPassword != _password) {
					ERRINCORRECTPASSWORD(clientSocket);
					return false;
				}
				gotPassword = true;
			}
			else if (cmd == "NICK") {
				std::getline(iss, receivedNickname, '\r');
				checkNC(receivedNickname);
				if (!isValidNickname(receivedNickname, clients, clientSocket)) {
					return false;
				}
				else if (receivedNickname.empty()) {
					ERRMOREPARAMS(clientSocket, receivedNickname, cmd);
					return false;
				}
				gotNick = true;
			}
			else if (cmd == "USER") {
				std::getline(iss, receivedUsername, ' ');
				if (checkNC(receivedUsername) == true) {
					gotEnd = true;
				}
				if (receivedUsername.empty()) {
					ERRMOREPARAMS(clientSocket, receivedNickname, cmd);
					return false;
				}
			}
			else {
				ERRUNKNOWNCOMMAND(clientSocket, "", cmd);
			}
			std::getline(iss, cmd, '\n');
		}
	}
	if (!gotPassword || !gotNick) {
		ERRMOREPARAMS(clientSocket, "", "Missing NICK or PASS commands");
		return false;
	}
	WELCOME_001(receivedNickname, clientSocket);
	clients[receivedNickname] = new Client(clientSocket, receivedUsername, receivedNickname);
	std::cout << GREEN "Client : {" << receivedNickname << ", " << receivedUsername << ", " << clientSocket << "} successfully connected." EOC << std::endl;
	return true;
}
