/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/16 12:25:30 by thibnguy          #+#    #+#             */
/*   Updated: 2024/04/23 20:16:29 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << YELLOW "[Usage]: " WHITE "./ircserv <port> <password>" EOC << std::endl;
		return EXIT_FAILURE;
	}

	std::string port = argv[1];
	std::string password = argv[2];

	Ircserv server(port, password);
	server.initServer();
	server.runServer();

	return 0;
}
