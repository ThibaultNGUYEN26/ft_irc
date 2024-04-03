/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 16:12:54 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/03 16:06:49 by thibnguy         ###   ########.fr       */
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
	freeaddrinfo(_server.res); // Ensure resources are freed
	close(_server.sfd); // Close the server socket
}

void Ircserv::initServer() {
	// Initiliazing address structs with getaddrinfo()
	std::stringstream	ss;

	ss << _port;
	memset(&(_server.hints), 0, sizeof _server.hints);
	_server.hints.ai_family = AF_UNSPEC;
	_server.hints.ai_socktype = SOCK_STREAM;
	_server.hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, (ss.str()).c_str(), &_server.hints, &_server.res) < 0) {
		error("Failure to call getaddrinfo().");
	}
	// Opening, binding and listening on the socket
	_server.sfd = socket((_server.res)->ai_family, (_server.res)->ai_socktype, (_server.res)->ai_protocol);
	if (_server.sfd < 0) {
		error("Failure to open socket.");
	}
	if (bind(_server.sfd, (_server.res)->ai_addr, (_server.res)->ai_addrlen) < 0) {
		error("Failure to bind socket.");
	}
	listen(_server.sfd, BACKLOG);
}

void Ircserv::runServer() {
	std::cout << "Port: " << _port << " | Password: " << _password << std::endl;
	// Array of pollfd structures for monitoring file descriptors
	std::vector<struct pollfd> fds;

	// Add the server socket to the monitoring set for incoming connections (POLLIN event)
	struct pollfd listen_fd;
	listen_fd.fd = _server.sfd;
	listen_fd.events = POLLIN;
	listen_fd.revents = 0;
	fds.push_back(listen_fd);

	while (true) {
		// Wait for an event on any of the monitored file descriptors
		int ret = poll(fds.data(), fds.size(), -1); // -1 means no timeout
		if (ret < 0) {
			error("poll failure");
			exit(EXIT_FAILURE);
		}

		for (size_t i = 0; i < fds.size(); i++) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == _server.sfd) {
					// Listening socket is ready, meaning an incoming connection
					struct sockaddr_storage clientAddr;
					socklen_t clientAddrSize = sizeof(clientAddr);
					int clientSocket = accept(_server.sfd, (struct sockaddr *)&clientAddr, &clientAddrSize);
					if (clientSocket < 0) {
						error("accept failure");
						continue;
					}

					// Right after accepting the connection...
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

					if (bytesRead <= 0) {
						// Handle error or closed connection
						std::cout << "Failed to receive data or connection closed." << std::endl;
						close(clientSocket);
					} else {
						buffer[bytesRead - 1] = '\0'; // Null-terminate the received string
						std::string receivedCommand(buffer);

						// Assuming a custom protocol where the client sends the password directly
						std::cout << receivedCommand << "\n" << _password << std::endl;
						if (receivedCommand == _password) {
							std::cout << "Password correct. Client connected." << std::endl;

							// If password is correct, proceed to add the client socket to the monitoring set...
							struct pollfd client_fd;
							client_fd.fd = clientSocket;
							client_fd.events = POLLIN; // Monitor for reading
							fds.push_back(client_fd);
						} else {
							std::cout << "Incorrect password. Connection refused." << std::endl;
							close(clientSocket);
						}
					}
				} else {
					// Existing client socket is ready for reading
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					ssize_t bytesRead = read(fds[i].fd, buffer, sizeof(buffer) - 1);

					if (bytesRead > 0) {
						// Here you would handle further commands from the client
						std::cout << "Received message: " << buffer << std::endl;
					} else {
						// Connection closed by client or error reading
						close(fds[i].fd);
						fds.erase(fds.begin() + i); // Remove from set
						std::cout << "Client disconnected." << std::endl;
						--i; // Adjust index after removal
					}
				}
			}
		}
	}
}
