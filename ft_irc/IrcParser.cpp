#include "Ircserv.hpp"

/* void capLsCommand() {
	("CAP LS {version}");
} */

void passCommand(std::string &_password, struct t_server &_server, int &clientSocket) {
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
		std::cout << receivedCommand << std::endl;
		// Assuming a custom protocol where the client sends the password directly
		if (receivedCommand == _password) {
			std::cout << "Password correct. Client connected." << std::endl;

			// If password is correct, proceed to add the client socket to the monitoring set...
			struct pollfd client_fd;
			client_fd.fd = clientSocket;
			client_fd.events = POLLIN; // Monitor for reading
			_server.fds.push_back(client_fd);
		} else {
			std::cout << "Incorrect password. Connection refused." << std::endl;
			close(clientSocket);
		}
	}
}
