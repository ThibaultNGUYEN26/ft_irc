/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 16:12:54 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/03 18:55:23 by thibnguy         ###   ########.fr       */
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
	if (getaddrinfo("127.0.0.1", (ss.str()).c_str(), &_server.hints, &_server.res) < 0) {
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
					std::cout << "allo je suis la\n";
					// Listening socket is ready, meaning an incoming connection
					struct sockaddr_storage clientAddr;
					socklen_t clientAddrSize = sizeof(clientAddr);
					int clientSocket = accept(_server.sfd, (struct sockaddr *)&clientAddr, &clientAddrSize);
					if (clientSocket < 0) {
						error("accept failure");
						continue;
					}

					passCommand(_password, _server, clientSocket);
				} else {
					// Existing client socket had an event that needs to be read
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					ssize_t bytesRead = read((_server.fds[i]).fd, buffer, sizeof(buffer) - 1);

					if (bytesRead > 0) {
						// Here you would handle further commands from the client
						std::cout << "Received message: " << buffer << std::endl;
					} else {
						// Connection closed by client or error reading
						close((_server.fds[i]).fd);
						(_server.fds).erase((_server.fds).begin() + i); // Remove from set
						std::cout << "Client disconnected." << std::endl;
						--i; // Adjust index after removal
					}
				}
			}
		}
	}
}
