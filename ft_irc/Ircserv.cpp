/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 16:12:54 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/05 21:40:17 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

static void error(const std::string &msg)
{
	std::cerr << RED "[Error] " WHITE << msg << EOC << std::endl;
	exit(EXIT_FAILURE);
}

Ircserv::Ircserv(std::string &port, std::string &password) : _password(password) {
	std::stringstream ss_port(port);
	
	ss_port >> _port;
	if (ss_port.fail() || !ss_port.eof()) {
		error("Wrong port format.");
	}
	if (_port < 0 || _port > 9999) {
		error("Port out of range.");
	}
}

Ircserv::~Ircserv() {
	//freeaddrinfo(_server.res); // Ensure resources are freed
	close(_server.sfd); // Close the server socket
}

void Ircserv::initServer() {
	// Initiliazing address structs with getaddrinfo()
	_server.sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server.sfd < 0) {
		error("Failure to open socket.");
	}
	_server.hints.sin_family = AF_INET;
	_server.hints.sin_addr.s_addr = inet_addr("127.0.0.1");
	_server.hints.sin_port = htons(_port);

	// Opening, binding and listening on the socket
	int enable = 1;
	if (setsockopt(_server.sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&enable, sizeof(enable)))
		error("Failure to execute setsockopt().");
	if (bind(_server.sfd, (struct sockaddr*)&_server.hints, sizeof(_server.hints)) < 0) {
		error("Failure to bind to port.");
	}
	if (listen(_server.sfd, BACKLOG) < 0) {
		error("Failure to listen on socket.");
	}
	/* Add the server listening socket to the monitoring set for incoming connections
		Initialization :
		- POLLIN event => there is data to read
		- revents = 0 => there are no previous events that have occured on this file descriptor
	*/
	struct pollfd listen_fd;
	listen_fd.fd = _server.sfd;
	listen_fd.events = POLLIN;
	listen_fd.revents = 0;
	(_server.fds).push_back(listen_fd);
}

bool validateClientPassword(int clientSocket, const std::string& _password) {
	bool	gotPassword = false, gotUsername = false, gotNickname = false;
	std::string nick;

	while (!gotNickname || !gotUsername || !gotPassword)
	{
		char buffer[1024];

		memset(buffer, 0, sizeof(buffer));
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead <= 0) {
			std::cout << "Failed to receive data or connection closed." << std::endl;
			return false; // Failed to read data
		}
		buffer[bytesRead] = '\0'; // Null-terminate the received string
		// std::cout << "*" << buffer << "*" << std::endl;

		std::string receivedCommand(buffer);
		std::istringstream iss(receivedCommand);
		std::string	cmd;

		while (std::getline(iss, cmd, ' ')) {
			// std::cout << "-" << cmd << "-" << std::endl;
			if (cmd == "CAP") {
				std::getline(iss, cmd, '\r');
				// std::cout << "--" << cmd << "--" << std::endl;
			}
			else if (cmd == "PASS") {
				std::string	receivedPassword;
				std::getline(iss, receivedPassword, '\r');
				// std::cout << "--" << receivedPassword << "--" << std::endl;
				if (receivedPassword != _password) {
					std::cout << "Incorrect password. Connection refused." << receivedPassword << std::endl;
					return false;
				}
				gotPassword = true;
			}
			else if (cmd == "NICK") {
				std::string	receivedNickname;
				std::getline(iss, receivedNickname, '\r');
				// std::cout << "--" << receivedNickname << "--" << std::endl;
				nick = receivedNickname;
				gotNickname = true;
				// std::cout << "--" << cmd << "--" << std::endl;
			}
			else if (cmd == "USER") {
				std::string	receivedUsername;
				std::getline(iss, receivedUsername, ' ');
				// std::cout << "--" << receivedUsername << "--" << std::endl;
				gotUsername = true;
				break;
			}
			std::getline(iss, cmd, '\n');
		}
	}
	std::cout << GREEN "Client " << nick << " successfully connected." EOC << std::endl;
	return true;
}


void Ircserv::runServer() {
	std::cout << WHITE "Port: " BLUE << _port << WHITE " | Password: " BLUE << _password << EOC << std::endl;

	while (true) {
		/*
			Wait for an event on any of the monitored file descriptors using poll()
			- -1 means no timeout and waits indefinetly for an event to occur on the fds in the pollfd structure
			- return value : < 0 means failure, 0 means no events, > 0 means events occured
		*/
		int ret = poll((_server.fds).data(), (_server.fds).size(), -1); // -1 means no timeout
		if (ret < 0) {
			error("poll failure");
			exit(EXIT_FAILURE);
		}
		// We go through each file descriptor in the pollfd structure
		for (size_t i = 0; i < (_server.fds).size(); i++) {
			// If revents in fd[i] (events occured) and the event is data that is read
 			if ((_server.fds[i]).revents & POLLIN) {
				// If the POLLIN event that occured is in the server's fd => potential client trying to connect
				if ((_server.fds[i]).fd == _server.sfd) {
					// Listening socket is ready, meaning an incoming connection
					struct sockaddr_storage clientAddr;
					socklen_t clientAddrSize = sizeof(clientAddr);
					int clientSocket = accept(_server.sfd, (struct sockaddr *)&clientAddr, &clientAddrSize);
					if (clientSocket < 0) {
						error("accept failure");
						continue;
					}
					if (!validateClientPassword(clientSocket, _password)) {
						close(clientSocket); // Close connection if password validation fails
						continue; // Skip further processing for this client
					}
					pollfd newfd;
					newfd.fd = clientSocket;
					newfd.events = POLLIN;
					_server.fds.push_back(newfd);
				} else {
					// Existing client socket had an event that needs to be read
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					ssize_t bytesRead = read((_server.fds[i]).fd, buffer, sizeof(buffer) - 1);

					if (bytesRead > 0) {
						buffer[bytesRead] = '\0'; // Ensure null-terminated
						std::string command(buffer);
					} else {
						// Connection closed by client or error reading
						close((_server.fds[i]).fd);
						(_server.fds).erase((_server.fds).begin() + i); // Remove from set
						std::cout << BLUE "Client disconnected." EOC << std::endl;
						--i; // Adjust index after removal
					}
				}
			}
		}
	}
}
