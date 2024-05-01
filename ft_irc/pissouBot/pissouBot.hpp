/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pissouBot.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 19:32:54 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/01 20:53:16 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstring>
#include <netdb.h>
#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sstream>
#include <cstdlib>
#include <sstream>
#include <unistd.h>

#define EOC "\033[1;1;0m"
#define MAGENTA "\033[1;35m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"

class pissouBot {

public:

	pissouBot(std::string &port, std::string &password);
	~pissouBot();

	void	initClient();

private:

	int			_port;
	std::string _password;
};
