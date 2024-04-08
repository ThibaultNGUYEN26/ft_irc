/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 16:12:54 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/08 18:57:16 by rchbouki         ###   ########.fr       */
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
	for (std::map<std::string, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
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

bool	Ircserv::isValidNickname(const std::string& nickname) {
	if (nickname.length() > 9) {
		std::cout << RED "Nickname's length exceeds 9 characters. Change it before registering again." EOC << std::endl;	
		return false;
	}
	if (_clients.find(nickname) != _clients.end()) {
		std::cout << RED "Nickname already exists within the server's clients. Change it before registering again." EOC << std::endl;
		return false;
	}
	return true;
}

bool	Ircserv::validateClientCommands(int& clientSocket, const std::string& _password) {
	bool	gotPassword = false, gotUsername = false, gotNickname = false;
	std::string	receivedNickname;
	std::string	receivedUsername;

	while (!gotNickname || !gotUsername || !gotPassword)
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
				std::getline(iss, cmd, '\r');
			}
			else if (cmd == "PASS") {
				std::string	receivedPassword;
				std::getline(iss, receivedPassword, '\r');
				if (receivedPassword != _password) {
					std::cout << "Incorrect password. Connection refused." << receivedPassword << std::endl;
					return false;
				}
				gotPassword = true;
			}
			else if (cmd == "NICK") {
				std::getline(iss, receivedNickname, '\r');
				if (!isValidNickname(receivedNickname)) {
					return false;
				}
				gotNickname = true;
			}
			else if (cmd == "USER") {
				std::getline(iss, receivedUsername, ' ');
				gotUsername = true;
				std::string welcome = "001 " + receivedNickname + " :" MAGENTA "Welcome to Titi&Riri's IRC serv" EOC "\r\n";
				send(clientSocket, welcome.c_str(), welcome.length(), 0);
				break;
			}
			std::getline(iss, cmd, '\n');
		}
	}
	_clients[receivedNickname] = new Client(clientSocket, receivedUsername, receivedNickname);

	std::cout << GREEN "Client : {" << receivedNickname << ", " << receivedUsername << ", " << clientSocket << "} successfully connected." EOC << std::endl;
	/* // DISPLAY _clients
	for (std::map<std::string, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		std::cout << "Client : {" << (it->second)->getNickname() << ", " << (it->second)->getUsername() << ", " << (it->second)->getSocket() << "}" << std::endl;
	} */
	return true;
}

void	Ircserv::eraseClient(int &clientSocket) {
	for (std::map<std::string, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			std::cout << BLUE "Client : {" << (it->second)->getNickname() << ", " << (it->second)->getUsername() << ", " << (it->second)->getSocket() << "} disconnected." EOC << std::endl;
			delete(it->second);
			_clients.erase(_clients.find(it->first));
			break;
		}
	}
}

void	Ircserv::handleJoinCommand(int clientSocket, const std::string& channelName) {
	// Check if the channel exists, create it if not
	std::map<std::string, std::vector<int> >::iterator it = _channels.find(channelName);
	if (it == _channels.end()) {
		// Since we can't directly emplace in C++98, we create the vector first
		std::vector<int> clientSockets;
		clientSockets.push_back(clientSocket);
		_channels[channelName] = clientSockets;
		//_channels.insert(std::make_pair(channelName, clientSockets));
	} else {
		// Channel exists, add the client to the channel
		it->second.push_back(clientSocket);
	}

	// Notify all clients in the channel about the new member
	std::string nickname;
	for (std::map<std::string, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			nickname = (it->second)->getNickname();
			break;
		}
	}
	
	// Send JOIN message back to the client to confirm
	std::string joinConfirm = ":" + nickname + "!~user@host JOIN :" + channelName + "\r\n";
	send(clientSocket, joinConfirm.c_str(), joinConfirm.size(), 0);

	std::string joinMsg = ":" + nickname + "!~user@host JOIN :" + channelName + "\r\n";
	std::vector<int>& members = _channels[channelName];
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != clientSocket) { // Don't send notification to the user who just joined
			send(*memberIt, joinMsg.c_str(), joinMsg.size(), 0);
		}
	}
}

void	Ircserv::broadcastToChannel(int senderSocket, const std::string& channelName, const std::string& message) {
	// Iterate over clients to find the nickname of the sender.
	std::string nickname;
	for (std::map<std::string, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getSocket() == senderSocket) {
			nickname = it->second->getNickname();
			break;
		}
	}
	// Find the channel in the map
	std::map<std::string, std::vector<int> >::iterator channelIt = _channels.find(channelName);
	if (channelIt == _channels.end()) {
		std::string fullMessage = nickname + " " + channelName + " :" + "No such channel" + "\r\n";
		send(senderSocket, fullMessage.c_str(), fullMessage.length(), 0);
		return ;
	}
	// Construct the message using the sender's nickname.
	std::string fullMessage = ":" + nickname + "!~user@host PRIVMSG " + channelName + " :" + message + "\r\n";
	// Broadcast the message to all channel members except the sender.
	std::vector<int>& members = channelIt->second;
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != senderSocket) { // Exclude the message sender
			send(*memberIt, fullMessage.c_str(), fullMessage.length(), 0);
		}
	}
}

void	Ircserv::sendDM(int senderSocket, const std::string& target, const std::string& message) {
	// Check if the target exists
 	std::map<std::string, Client*>::iterator it = _clients.find(target);
	int	targetSocket;
	if (it == _clients.end()) {
		std::string fullMessage = target + " : There was no such channel\r\n";
		send(senderSocket, fullMessage.c_str(), fullMessage.length(), 0);
		return ;
	}
	else
	{
		targetSocket = _clients[target]->getSocket();
	}
	// retrieve nickname of the sender
	std::string nickname;
	for (std::map<std::string, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getSocket() == senderSocket) {
			nickname = it->second->getNickname();
			break;
		}
	}
	std::string fullMessage = ":" + nickname + " PRIVMSG " + target + " :" + message + "\r\n";
	std::cout << fullMessage << std::endl;
	send(targetSocket, fullMessage.c_str(), fullMessage.length(), 0);
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
					ssize_t bytesRead = read((_server.fds[i]).fd, buffer, sizeof(buffer) - 1);

					if (bytesRead > 0) {
						buffer[bytesRead] = '\0'; // Ensure null-terminated
						std::string command(buffer);

						// Print the received command to the console
						std::cout << "Received command from client: " << buffer << std::endl;
						
						if (command.find("JOIN") == 0) {
							// Extract the channel name from the command
							std::istringstream iss(command);
							std::string channelName;
							std::getline(iss, channelName, ' ');
							std::getline(iss, channelName, '\r');
							handleJoinCommand(_server.fds[i].fd, channelName); // Handle the JOIN command
						}

						if (command.find("PRIVMSG") == 0) {
							// Extract the target (channel or user) and the message
							std::istringstream iss(command);
							std::string	target;
							std::getline(iss, target, ' ');
							std::getline(iss, target, ' ');
							std::string	message;
							std::getline(iss, message, ':');
							std::getline(iss, message, '\r');
							std::cout << "Message: " << message << std::endl;
							if (target[0] == '#')
								broadcastToChannel(_server.fds[i].fd, target, message);
							else
								sendDM(_server.fds[i].fd, target, message);
						}

					} else {
						// Connection closed by client or error reading
						(_server.fds).erase((_server.fds).begin() + i);
						eraseClient((_server.fds[i]).fd);
						close((_server.fds[i]).fd);
						--i;
					}
				}
			}
		}
	}
}
