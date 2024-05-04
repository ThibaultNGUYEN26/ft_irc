/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pissouBot.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 19:31:17 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 13:28:37 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pissouBot.hpp"

static void error(const std::string &msg)
{
	std::cerr << RED "[Error] " WHITE << msg << EOC << std::endl;
	exit(EXIT_FAILURE);
}

pissouBot::pissouBot(std::string &port, std::string &password) : _password(password) {
	std::stringstream ss_port(port);
	
	ss_port >> _port;
	if (ss_port.fail() || !ss_port.eof()) {
		error("Wrong port format.");
	}
	if (_port < 1024 || _port > 5000) {
		error("Port out of range.");
	}
}

pissouBot::~pissouBot() {}

void pissouBot::initClient() {
	// Create a socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("Error in creating socket.");
		return ;
	}
	// Server address
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(_port);

	// Connect to the server
	if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
		error("Error connecting to server.");
		close(sockfd);
		return ;
	}

	std::string passMsg = "PASS " + _password + "\r\n";
	send(sockfd, passMsg.c_str(), passMsg.length(), 0);

	std::string nickMsg = "NICK pissouBot\r\n";
	send(sockfd, nickMsg.c_str(), nickMsg.length(), 0);

	std::string userMsg = "USER pissouBot\n";
	send(sockfd, userMsg.c_str(), userMsg.length(), 0);

	char buffer[1024];
	int numBytes = 0;

	while (true) {
		memset(buffer, 0, sizeof(buffer));
		numBytes = recv(sockfd, buffer, sizeof(buffer), 0);
		if (numBytes > 0) {
			ft_pissouBot(buffer, sockfd);
		} else if (numBytes == 0) {
			std::cout << RED "Connection closed by server." EOC << std::endl;
			break;
		} else {
			error("recv failed");
			break;
		}
	}

	close(sockfd);
}

