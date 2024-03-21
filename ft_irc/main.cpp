/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/16 12:25:30 by thibnguy          #+#    #+#             */
/*   Updated: 2024/03/16 12:35:38 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << YELLOW "[Usage]: " WHITE "./ircserv <port> <password>" EOC << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << WHITE "Port: " BLUE << argv[1] << WHITE " | Password: " BLUE << argv[2] << EOC << std::endl;
    return 0;
}
