/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 16:12:54 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 20:09:14 by thibnguy         ###   ########.fr       */
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
	if (_port < 1024 || _port > 5000) {
		error("Port out of range.");
	}
}

Ircserv::~Ircserv() {
	for (clientMap::iterator it = _clients.begin(); it != _clients.end(); it++) {
		delete(it->second);
		_clients.erase(_clients.find(it->first));
	}
	for (channelMap::iterator it = _channels.begin(); it != _channels.end(); it++) {
		delete(it->second);
		_channels.erase(_channels.find(it->first));
	}
	for (size_t i = 0; i < (_server.fds).size(); i++) {
		close((_server.fds[i]).fd);
	}
}

void	Ircserv::initServer() {
	// Initiliazing address structs with getaddrinfo()
	_server.sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server.sfd < 0) {
		error("Failure to open socket.");
	}
	_server.hints.sin_family = AF_INET;
	_server.hints.sin_addr.s_addr = htonl(INADDR_ANY);
	_server.hints.sin_port = htons(_port);
	// Opening, binding and listening on the socket
	int enable = 1;
	if (setsockopt(_server.sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&enable, sizeof(enable)))
		error("Failure to execute setsockopt().");
	fcntl(_server.sfd, F_SETFL, O_NONBLOCK);
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
	pollfd listen_fd = {_server.sfd, POLLIN, 0};
	(_server.fds).push_back(listen_fd);
}

void	Ircserv::eraseClient(int &clientSocket) {
	for (clientMap::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			std::cout << BLUE "Client : {" << (it->second)->getNickname() << ", " << (it->second)->getUsername() << ", " << (it->second)->getSocket() << "} disconnected." EOC << std::endl;
			delete(it->second);
			_clients.erase(_clients.find(it->first));
			break;
		}
	}
}

void signalHandler(int signum) {
	std::cout << " Interrupt signal (" << signum << ") received.\n";
	exit(signum);
}

void Ircserv::runServer() {
	std::cout << WHITE "Port: " BLUE << _port << WHITE " | Password: " BLUE << _password << EOC << std::endl;

	std::string command;
	signal(SIGINT, signalHandler);
	while (true) {
		/*
			Wait for an event on any of the monitored file descriptors using poll()
			- -1 means no timeout and waits indefinetly for an event to occur on the fds in the pollfd structure
			- return value : < 0 means failure, 0 means no events, > 0 means events occured
		*/
		int ret = poll((_server.fds).data(), (_server.fds).size(), -1);
		if (ret < 0) {
			error("poll failure");
			exit(EXIT_FAILURE);
		}
		// We go through each file descriptor in the pollfd structure
		for (size_t i = 0; i < (_server.fds).size(); i++) {
			// If revents in fd[i] (events occured) and the event is data that is read
 			if ((_server.fds[i]).revents) {
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
					if (!registerClient(clientSocket, _password, _clients)) {
						close(clientSocket);
						continue;
					}
					pollfd newfd = {clientSocket, POLLIN, 0};
					_server.fds.push_back(newfd);
				}
				else {
					// Existing client socket had an event that needs to be read
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					ssize_t bytesRead = recv((_server.fds[i]).fd, buffer, sizeof(buffer) - 1, 0);
					if (bytesRead > 0) {
						buffer[bytesRead] = '\0';
						std::string commandBuffer(buffer);
						if (command.empty() && commandBuffer.find('\n') != std::string::npos) {
							command = commandBuffer;
						}
						// Print the received command to the console
						if (command.find('\n') != std::string::npos) {
							if (command.find("JOIN") == 0) {
								handleJoinCommand(command, _clients, _channels, _server.fds[i].fd);
							}
							else if (command.find("PART") == 0) {
								handleLeaveCommand(command, _clients, _channels, _server.fds[i].fd);
							}
							else if (command.find("KICK") == 0) {
								handleKickCommand(command, _clients, _channels, _server.fds[i].fd);
							}
							else if (command.find("PRIVMSG") == 0) {
								std::istringstream iss(command);
								std::string	target, message;
								std::getline(iss, target, ' ');
								std::getline(iss, target, ' ');
								std::getline(iss, message, ':');
								std::getline(iss, message, '\r');
								if (target[0] == '#')
									broadcastToChannel(_server.fds[i].fd, target, message, _clients, _channels);
								else
									sendDM(_server.fds[i].fd, target, message, _clients);
								// dcc send {target} {file}
								// dcc get {sender}
							}
							else if (command.find("TOPIC") == 0) {
								handleTopicCommand(command, _clients, _channels, _server.fds[i].fd);
							}
							else if (command.find("PING") == 0) {
								std::istringstream iss(command);
								std::string	token;
								std::getline(iss, token, ' ');
								std::getline(iss, token, '\r');
								std::string fullMessage = "PONG " + token + "\r\n";
								send((_server.fds[i]).fd, fullMessage.c_str(), fullMessage.length(), 0);
							}
							else if (command.find("QUIT") == 0) {
								eraseClient((_server.fds[i]).fd);
								close((_server.fds[i]).fd);
								(_server.fds).erase((_server.fds).begin() + i);
								--i;
							}
							else if (command.find("INVITE") == 0) {
								handleInviteCommand(command, _clients, _channels, _server.fds[i].fd);
							}
							else if (command.find("MODE") == 0) {
								handleModeCommand(command, _clients, _channels, _server.fds[i].fd);
							}
							else {
								std::istringstream iss(command);
								std::string cmd;
								std::getline(iss, cmd, ' ');
								checkNC(cmd);
								std::string nickname = (getClientIterator(_server.fds[i].fd, _clients)->second)->getNickname();
								ERRUNKNOWNCOMMAND(_server.fds[i].fd, nickname, cmd);
							}
							command = "";
						}
						else {
							command += commandBuffer;
						}
					} else {
						// Connection closed by client or error reading
						eraseClient((_server.fds[i]).fd);
						close((_server.fds[i]).fd);
						(_server.fds).erase((_server.fds).begin() + i);
						--i;
					}
				}
			}
		}
	}
}
