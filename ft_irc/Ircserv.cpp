#include "Ircserv.hpp"

Ircserv::Ircserv(std::string &port, std::string &password) : _password(password) {
	std::stringstream ss_port(port);
	
	ss_port >> _port;
	if (ss_port.fail() || !ss_port.eof()) {
		std::cerr << RED "[Error] " WHITE "Wrong port format." EOC << std::endl;
		exit(EXIT_FAILURE);
	}
	if (_port < 0 || _port > 9999) {
		std::cerr << RED "[Error] " WHITE "Port out of range." EOC << std::endl;
		exit(EXIT_FAILURE);
	}
}

Ircserv::~Ircserv() {}

void Ircserv::runServer() {
	std::cout << WHITE "Port: " BLUE << _port << WHITE " | Password: " BLUE << _password << EOC << std::endl;
}
