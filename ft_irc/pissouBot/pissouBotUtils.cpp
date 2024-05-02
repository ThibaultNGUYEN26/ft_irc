/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pissouBotUtils.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 19:19:54 by thibnguy          #+#    #+#             */
/*   Updated: 2024/05/02 20:28:46 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pissouBot.hpp"

static std::string toLower(const std::string& input) {
	std::string result = input;
	for (size_t i = 0; i < result.length(); ++i) {
		result[i] = std::tolower(static_cast<unsigned char>(result[i]));
	}
	return result;
}

template<typename T, size_t N>
std::vector<T> makeVector(const T (&arr)[N]) {
	return std::vector<T>(arr, arr + N);
}

static bool searchKeyword(const std::string& command, const std::vector<std::string>& words) {
	int total_count = 0;

	for (size_t i = 0; i < words.size(); ++i) {
		int count = 0;
		size_t pos = command.find(words[i], 0);

		while (pos != std::string::npos) {
			count++;
			pos = command.find(words[i], pos + words[i].length());
		}
		total_count += count;
	}

	return total_count > 0;
}

void	ft_pissouBot(char *buffer, int targetSocket) {
	std::istringstream iss(buffer);
	std::string target, CMD, bot, command;
	std::getline(iss, target, ':');
	std::getline(iss, target, ' ');
	std::getline(iss, CMD, ' ');
	std::getline(iss, bot, ' ');
	std::getline(iss, command, ':');
	std::getline(iss, command, '\r');

	command = toLower(command);
	static const std::string arr[] = {"hi", "hello", "yo", "good morning", "good evening", "what's up", "hey"};
	if (searchKeyword(command, makeVector(arr))) {
		std::string fullMessage = "PRIVMSG " + target + " :Hello " BLUE + target + EOC " how are you?\r\n";
		send(targetSocket, fullMessage.c_str(), fullMessage.length(), 0);
	}
}
