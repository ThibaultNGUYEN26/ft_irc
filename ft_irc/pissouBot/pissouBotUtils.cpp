/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pissouBotUtils.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 19:19:54 by thibnguy          #+#    #+#             */
/*   Updated: 2024/05/03 18:13:58 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pissouBot.hpp"

struct UserState {
	bool isPlaying;
	bool alreadyPlayed;
	bool bonus;
	int numberToGuess;
	int lives;

	UserState() : isPlaying(false), alreadyPlayed(false), numberToGuess(0), lives(5) {}
};

std::map<std::string, UserState> userStates;

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

// Function to search for a single keyword
static bool searchKeyword(const std::string& command, const std::string& word) {
	std::string modCommand = " " + command + " ";
	std::string modWord = " " + word + " ";
	return modCommand.find(modWord) != std::string::npos;
}

// Function to search for any of multiple keywords
static bool searchKeyword(const std::string& command, const std::vector<std::string>& words) {
	for (size_t i = 0; i < words.size(); ++i) {
		if (searchKeyword(command, words[i])) {
			return true;
		}
	}
	return false;
}

static void sendMsg(std::string target, int targetSocket, std::string msg) {
	std::string fullMessage = "PRIVMSG " + target + " :" + msg + "\r\n";
	send(targetSocket, fullMessage.c_str(), fullMessage.length(), 0);
}

// Function to start a new game
void startNewGame(const std::string& user) {
	userStates[user].isPlaying = true;
	userStates[user].alreadyPlayed = true;
	userStates[user].bonus = false;
	userStates[user].numberToGuess = rand() % 1000 + 1;
	// std::cout << userStates[user].numberToGuess << std::endl;
	userStates[user].lives = 8;
}

static int stringToInt(const std::string& str) {
	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] < '0' || str[i] > '9') {
			return (-1);
		}
	}
	std::istringstream iss(str);
	int nb;
	iss >> nb;
	return nb;
}

static std::string intToString(int nb) {
	std::ostringstream oss;
	oss << nb;
	return oss.str();
}

// Function to process game input
static void processGameInput(const std::string& user, const std::string& command, std::string target, int targetSocket) {
	if (!userStates[user].isPlaying) {
		return;
	}


	if (command == "je vais te mettre outstanding") {
		if (!userStates[user].bonus) {
			userStates[user].lives++;
			sendMsg(target, targetSocket,  GREEN "You have now " + intToString(userStates[user].lives) + " lives remaining." EOC);
			userStates[user].bonus = true;
			return ;
		} else {
			sendMsg(target, targetSocket, RED "You have already use your bonus !" EOC);
			return ;
		}
	}

	int guess = stringToInt(command);

	if (guess == -1) {
		sendMsg(target, targetSocket, RED "Error you have to enter a valid number." EOC);
		return ;
	} else if (guess > 1000 || guess < 1) {
		sendMsg(target, targetSocket, RED "Error number out of bounds." EOC);
		return ;
	}
	// Check if the guess is correct
	if (guess == userStates[user].numberToGuess) {
		sendMsg(target, targetSocket, GREEN "Correct! You've guessed the number!" EOC);
		userStates[user].isPlaying = false;
		system("flatpak run org.mozilla.firefox 2>/dev/null https://youtu.be/ThcUlL975Nw?t=61&autoplay=1");
	} else {
		userStates[user].lives--;

		// Provide feedback about the guess
		if (guess < userStates[user].numberToGuess) {
			sendMsg(target, targetSocket, "Too low! You have " + intToString(userStates[user].lives) + " lives remaining.");
		} else if (guess > userStates[user].numberToGuess) {
			sendMsg(target, targetSocket, "Too high! You have " + intToString(userStates[user].lives) + " lives remaining.");
		}

		// Check if the game should end
		if (userStates[user].lives <= 0) {
			sendMsg(target, targetSocket, RED "Game over! You're out of lives. The number was " + intToString(userStates[user].numberToGuess) + EOC);
			userStates[user].isPlaying = false;
			system("flatpak run org.mozilla.firefox 2>/dev/null https://www.youtube.com/watch?v=dQw4w9WgXcQ&autoplay=1");
		}
	}
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
	static const std::string greetings[] = {"hi", "hello", "yo", "good morning", "good evening", "what's up", "hey"};
	static const std::string gameList[] = {"game list", "games list", "games' list", "list of games"};

	if (userStates[target].isPlaying) {
		processGameInput(target, command, target, targetSocket);
	}
	else if (searchKeyword(command, "play again") && userStates[target].alreadyPlayed) {
		sendMsg(target, targetSocket, "Let's play Guess the number !");
		usleep(100000);
		sendMsg(target, targetSocket, "The rule is simple, you have to guess the number I'm thinking between 1 and 1000");
		usleep(100000);
		sendMsg(target, targetSocket, "Be careful, you have 8 lives !");
		startNewGame(target);
	}
	else {
		// Greetings
		if (searchKeyword(command, makeVector(greetings))) {
			sendMsg(target, targetSocket, "Hello " BLUE + target + EOC " how are you?");
			userStates[target].alreadyPlayed = false;
		}

		// Gives the list of games pissouBot have
		else if (searchKeyword(command, makeVector(gameList))) {
			static const std::string gamesList[] = {"Guess the number"};
			std::vector<std::string> games = makeVector(gamesList);
			sendMsg(target, targetSocket, BLUE "Games' list:" EOC);
			usleep(10000);
			for (size_t i = 0; i < games.size(); ++i) {
				sendMsg(target, targetSocket, "    - " GREEN + games[i] + EOC);
				usleep(10000);
			}
			userStates[target].alreadyPlayed = false;
		}

		// Play Guess the number
		else if (searchKeyword(command, "guess the number")) {
			sendMsg(target, targetSocket, "Let's play Guess the number !");
			usleep(100000);
			sendMsg(target, targetSocket, "The rule is simple, you have to guess the number I'm thinking between 1 and 1000");
			usleep(100000);
			sendMsg(target, targetSocket, "Be careful, you have 8 lives !");
			startNewGame(target);
		}
	}
}
