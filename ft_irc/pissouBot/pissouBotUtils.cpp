/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pissouBotUtils.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 19:19:54 by thibnguy          #+#    #+#             */
/*   Updated: 2024/05/04 20:04:03 by thibnguy         ###   ########.fr       */
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
static bool searchKeyword(std::string& command, const std::string& word) {
	std::string modCommand = " " + command + " ";
	std::string modWord = " " + word + " ";
	return modCommand.find(modWord) != std::string::npos;
}

// Function to search for any of multiple keywords
static bool searchKeyword(std::string& command, const std::vector<std::string>& words) {
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
			sendMsg(target, targetSocket, GREEN "You have now " + intToString(userStates[user].lives) + (userStates[user].lives == 1 ? " life " : " lives ") + "remaining." EOC);
			userStates[user].bonus = true;
			return;
		} else {
			sendMsg(target, targetSocket, RED "You have already used your bonus!" EOC);
			return;
		}
	}

	int guess = stringToInt(command);

	if (guess == -1) {
		sendMsg(target, targetSocket, RED "Error: You have to enter a valid number." EOC);
		return;
	} else if (guess > 1000 || guess < 1) {
		sendMsg(target, targetSocket, RED "Error: Number out of bounds." EOC);
		return;
	}

	if (guess == userStates[user].numberToGuess) {
		sendMsg(target, targetSocket, GREEN "Correct! You've guessed the number!" EOC);
		userStates[user].isPlaying = false;
		system("flatpak run org.mozilla.firefox 2>/dev/null https://youtu.be/ThcUlL975Nw?t=61&autoplay=1");
	} else {
		userStates[user].lives--;

		if (userStates[user].lives > 0) {
			if (guess < userStates[user].numberToGuess) {
				sendMsg(target, targetSocket, "Too low! You have " + intToString(userStates[user].lives) + (userStates[user].lives == 1 ? " life " : " lives ") + "remaining.");
			} else if (guess > userStates[user].numberToGuess) {
				sendMsg(target, targetSocket, "Too high! You have " + intToString(userStates[user].lives) + (userStates[user].lives == 1 ? " life " : " lives ") + "remaining.");
			}
		} else {
			sendMsg(target, targetSocket, RED "Game over! You're out of lives. The number was " + intToString(userStates[user].numberToGuess) + EOC);
			userStates[user].isPlaying = false;
			system("flatpak run org.mozilla.firefox 2>/dev/null https://www.youtube.com/watch?v=dQw4w9WgXcQ&autoplay=1");
		}
	}
}

static std::string searchCmd(const std::string& command) {
	std::istringstream iss(command);
	std::vector<std::string> tokens;
	tokens.reserve(16);
	std::string token;

	while (iss >> token) {
		tokens.push_back(token);
	}

	std::vector<std::string>::iterator it = std::find(tokens.begin(), tokens.end(), "search");

	std::string searchingWords;
	++it;
	if (it != tokens.end()) {
		searchingWords = *it++;
	}
	while (it != tokens.end()) {
		searchingWords += "+" + *it;
		++it;
	}

	return searchingWords;
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
	static const std::string commandsList[] = {"commands list", "command list", "commands' list", "command's list", "list of commands", "what can you do", "are you usefull"};
	static const std::string explorer[] = {"42explorer", "42 explorer"};

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

		// Gives the list of commands pissouBot have
		else if (searchKeyword(command, makeVector(commandsList))) {
			static const std::string commandList[] = {"Greetings", "Search (Google or YouTube)", "Guess the Number (game)","Random Number from-to (1-6 by default)" , "42 Explorer"};
			std::vector<std::string> commands = makeVector(commandList);
			sendMsg(target, targetSocket, BLUE "Commands' list:" EOC);
			usleep(100000);
			for (size_t i = 0; i < commands.size(); ++i) {
				sendMsg(target, targetSocket, "    - " GREEN + commands[i] + EOC);
				usleep(100000);
			}
			userStates[target].alreadyPlayed = false;
		}

		// Launch Explorer video
		else if (searchKeyword(command, makeVector(explorer))) {
			system("flatpak run org.mozilla.firefox 2>/dev/null https://42explorer.github.io");
		}

		// Search on YouTube or Google
		else if (searchKeyword(command, "search")) {
			if (searchKeyword(command, "youtube")) {
				std::string searchingWords = searchCmd(command);
				sendMsg(target, targetSocket, "Searching on " RED "YouTube" EOC "...");
				std::string research = "flatpak run org.mozilla.firefox 2>/dev/null https://www.youtube.com/search?q=" + searchingWords;
				system(research.c_str());
			} else {
				std::string searchingWords = searchCmd(command);
				sendMsg(target, targetSocket, "Searching on " GREEN "Google" EOC "...");
				std::string research = "flatpak run org.mozilla.firefox 2>/dev/null https://letmegooglethat.com/?q=" + searchingWords;
				system(research.c_str());
			}
		}

		// Play Guess the number
		else if (searchKeyword(command, "guess the number")) {
			sendMsg(target, targetSocket, "Let's play Guess the number !");
			usleep(100000);
			sendMsg(target, targetSocket, "The rule is simple, you have to guess the number I'm thinking between " RED "1 and 1000" EOC);
			usleep(100000);
			sendMsg(target, targetSocket, "Be careful, you have " RED "8" EOC " lives !");
			startNewGame(target);
		}

		// Random a number
		else if (searchKeyword(command, "random number")) {
			std::istringstream iss(command);
			std::vector<std::string> tokens;
			std::string token;

			while (iss >> token) {
				tokens.push_back(token);
			}

			std::vector<std::string>::iterator it = std::find(tokens.begin(), tokens.end(), "random");
			if (it != tokens.end() && ++it != tokens.end() && *it == "number" && ++it != tokens.end()) {
				std::string range = *it;

				size_t dashPos = range.find('-');
				if (dashPos != std::string::npos && dashPos > 0 && dashPos < range.length() - 1) {
					std::string fromStr = range.substr(0, dashPos);
					std::string toStr = range.substr(dashPos + 1);

					int from = atoi(fromStr.c_str());
					int to = atoi(toStr.c_str());

					if (to > from) {
						std::srand(static_cast<unsigned int>(std::time(0)));
						int randomNumber = from + std::rand() % (to - from + 1);

						std::ostringstream ss;
						ss << "Random number generated: " << randomNumber;
						sendMsg(target, targetSocket, ss.str());
						return;
					}
				}
			}
			sendMsg(target, targetSocket, RED "Error: Invalid or missing range. Format must be 'random number from-to' with 'to' greater than 'from'." EOC);
		}
	}
}
