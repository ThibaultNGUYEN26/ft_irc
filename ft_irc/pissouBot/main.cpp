/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 19:41:21 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/03 15:38:23 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pissouBot.hpp"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << YELLOW "[Usage]: " WHITE "./pissouBot <port> <password>" EOC << std::endl;
		return EXIT_FAILURE;
	}

	std::string port = argv[1];
	std::string password = argv[2];

	srand(time(NULL));

	pissouBot pissou(port, password);
	pissou.initClient();
	
	return 0;
}

