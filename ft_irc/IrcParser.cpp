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

static void error(const std::string &msg)
{
	std::cerr << RED "[Error] " WHITE << msg << EOC << std::endl;
	exit(EXIT_FAILURE);
}

std::string receiveNick(int clientSocket) {
    char buffer[1024]; // Assuming a maximum message length of 1024 characters
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        // Handle error or connection closed by client
        return "";
    }
    buffer[bytesRead] = '\0'; // Null-terminate the received data
    std::string nickname(buffer);
    // You may want to perform additional validation on the received nickname
    return nickname;
}

void nickCommand(struct t_server &_server, int &clientSocket) {
	std::string command = "NICK " + receiveNick(clientSocket) + "\r\n";
	if (send(_server.sfd, command.c_str(), command.length(), 0) < 0) {
		error("Error sending NICK command");
	}
}

void userCommand(const std::string& username, const std::string& hostname, const std::string& servername, const std::string& realname, int sockfd) {
	std::string command = "USER " + username + " " + hostname + " " + servername + " :" + realname + "\r\n";
	if (send(sockfd, command.c_str(), command.length(), 0) < 0) {
		error("Error sending USER command");
	}
}

void handleRegistration(int sockfd) {
	// Receive client's registration information
	char buffer[1024]; // Assuming a maximum message length of 1024 characters
	ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
	if (bytesRead < 0) {
		error("Error receiving registration information");
		return;
	}

	// Parse client's registration information
	std::string registrationInfo(buffer, bytesRead);
	std::istringstream iss(registrationInfo);
	std::string username, hostname, servername, realname;
	iss >> username >> hostname >> servername >> realname;


	// Send USER command to IRC server
	userCommand(username, hostname, servername, realname, sockfd);
}
