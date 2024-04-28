/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 16:12:54 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/28 17:37:08 by rchbouki         ###   ########.fr       */
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
	for (clientMap::iterator it = _clients.begin(); it != _clients.end(); it++) {
		delete(it->second);
		_clients.erase(_clients.find(it->first));
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
	pollfd listen_fd = {_server.sfd, POLLIN, 0};
	(_server.fds).push_back(listen_fd);
}

bool	Ircserv::validateClientCommands(int& clientSocket, const std::string& _password) {
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
					std::string passFail = ":localhost 464  :Password incorrect\r\n";
					send(clientSocket, passFail.c_str(), passFail.size(), 0);
					return false;
				}
				gotPassword = true;
			}
			else if (cmd == "NICK") {
				std::getline(iss, receivedNickname, '\r');
				if (!isValidNickname(receivedNickname, _clients, clientSocket)) {
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
	std::string welcome = "001 " + receivedNickname + " :" MAGENTA "Welcome to Titi&Riri's IRC serv" EOC "\r\n";
	send(clientSocket, welcome.c_str(), welcome.length(), 0);
	_clients[receivedNickname] = new Client(clientSocket, receivedUsername, receivedNickname);
	std::cout << GREEN "Client : {" << receivedNickname << ", " << receivedUsername << ", " << clientSocket << "} successfully connected." EOC << std::endl;
	return true;
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

void Ircserv::runServer() {
	std::cout << WHITE "Port: " BLUE << _port << WHITE " | Password: " BLUE << _password << EOC << std::endl;

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
					if (!validateClientCommands(clientSocket, _password)) {
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
						std::string command(buffer);
						// Print the received command to the console
						std::cout << "Received command from client: " << buffer << std::endl;
						if (command.find("JOIN") == 0) {
							// Extract the channel name from the command
							command = command.substr(5, command.length());
							std::istringstream iss(command);
							std::string	channels, keys, channelName, key;
							std::getline(iss, channels, ' ');
							std::getline(iss, keys, '\r');
							std::cout << keys << std::endl;
							std::istringstream ssChannel(channels), ssKey(keys);
							while (std::getline(ssChannel, channelName, ',')) {
								if (channelName[channelName.length() - 1] == '\n') {
								channelName = channelName.substr(0, channelName.length() - 2);
								}
								else {
									std::getline(ssKey, key, ',');
								}
								std::cout << "Name *" << channelName << "*, key *" << key << "*\n"; 
								handleJoinCommand(_server.fds[i].fd, channelName, key, _clients, _channels);
							}
						}
						else if (command.find("PART") == 0) {
							// Extract the channel name from the command
							std::istringstream iss(command);
							std::string channelName;
							std::getline(iss, channelName, ' ');
							std::getline(iss, channelName, '\r');
							handleLeaveCommand(_server.fds[i].fd, channelName, _clients, _channels);
						}
						else if (command.find("KICK") == 0) {
							std::istringstream iss(command);
							std::string channelName, userToKick, reason;
							std::getline(iss, channelName, ' ');
							std::getline(iss, channelName, ' ');
							std::getline(iss, userToKick, ' ');
							std::getline(iss, reason, ':');
							std::getline(iss, reason, '\r');
							// Trim any leading spaces from the reason
							size_t startPos = reason.find_first_not_of(" ");
							if (startPos != std::string::npos) {
								reason = reason.substr(startPos);
							}
							handleKickCommand(_server.fds[i].fd, channelName, userToKick, reason, _clients, _channels);
						}
						else if (command.find("PRIVMSG") == 0) {
							std::istringstream iss(command);
							std::string	target, message;
							std::getline(iss, target, ' ');
							std::getline(iss, target, ' ');
							std::getline(iss, message, ':');
							std::getline(iss, message, '\r');
							std::cout << "Message: " << message << std::endl;
							if (target[0] == '#')
								broadcastToChannel(_server.fds[i].fd, target, message, _clients, _channels);
							else
								sendDM(_server.fds[i].fd, target, message, _clients);
						}
						else if (command.find("TOPIC") == 0) {
							std::istringstream iss(command);
							std::string channelName, topic;
							std::getline(iss, channelName, ' ');
							std::getline(iss, channelName, ' ');
							std::getline(iss, topic, ':');
							std::getline(iss, topic, '\r');
							std::cout << "Topic : " << topic << " in Channel : " << channelName << std::endl;
							handleTopicCommand(_server.fds[i].fd, channelName, topic, _clients, _channels);
						}
						else if (command.find("PING") == 0) {
							std::istringstream iss(command);
							std::string	token;
							std::getline(iss, token, ' ');
							std::getline(iss, token, '\r');
							std::string fullMessage = "PONG " + token + "\r\n";
							std::cout << fullMessage << std::endl;
							send((_server.fds[i]).fd, fullMessage.c_str(), fullMessage.length(), 0);
						}
						else if (command.find("QUIT") == 0) {
							eraseClient((_server.fds[i]).fd);
							close((_server.fds[i]).fd);
							(_server.fds).erase((_server.fds).begin() + i);
							--i;
						}
						else if (command.find("INVITE") == 0) {
							std::istringstream iss(command);
							std::string	nickname, channelName;
							std::getline(iss, nickname, ' ');
							std::getline(iss, nickname, ' ');
							std::getline(iss, channelName, '\r');
							std::cout << "Channel: " << channelName << " nickname: " << nickname << std::endl;
							handleInviteCommand(_server.fds[i].fd, channelName, nickname, _clients, _channels);
						}
						else if (command.find("MODE") == 0) {
							std::istringstream iss(command);
							std::string channelName, modeCommand, param;
							std::getline(iss, channelName, ' ');
							std::getline(iss, channelName, ' ');
							if (command.length() - 6 - channelName.length() != 4) {
								std::getline(iss, modeCommand, ' ');
								std::getline(iss, param, '\r');
							} else {
								std::getline(iss, modeCommand, '\r');
							}
							size_t startModePos = modeCommand.find_first_not_of(" ");
							size_t startParamPos = param.find_first_not_of(" ");
							if (startModePos != std::string::npos) {
								modeCommand = modeCommand.substr(startModePos);
							}
							if (startParamPos != std::string::npos) {
								param = param.substr(startParamPos);
							}
							std::cout << "Channel: " << channelName << " Mode: " << modeCommand << " Param: " << param << std::endl;
							handleModeCommand(_server.fds[i].fd, channelName, modeCommand, param, _clients, _channels);
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
