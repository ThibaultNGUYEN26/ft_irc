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

Ircserv::~Ircserv() {}

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
	std::cout << WHITE "Port: " BLUE << _port << WHITE " | Password: " BLUE << _password << EOC << std::endl;

	// Accept incoming connections
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrSize = sizeof clientAddr;
    int clientSocket;

    while (true) {
        clientSocket = accept(_server.sfd, (struct sockaddr *)&clientAddr, &clientAddrSize);
        if (clientSocket < 0) {	
            error("Failure to accept connection.");
        }	
		// Now we have a connected socket in clientSocket to communicate with the IRC client.
        // You can handle this connection as needed, e.g., by creating a new thread to handle the client or using non-blocking I/O.
    }

	freeaddrinfo(_server.res);
}
