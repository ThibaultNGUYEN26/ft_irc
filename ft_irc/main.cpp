/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/16 12:25:30 by thibnguy          #+#    #+#             */
/*   Updated: 2024/03/21 15:33:32 by thibnguy         ###   ########.fr       */
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
	server.runServer();
	
	return 0;
}
