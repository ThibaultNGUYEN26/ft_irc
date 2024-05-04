/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 18:13:58 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/04 16:38:22 by thibnguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcUtils.hpp"

void	ERRUNKNOWNCOMMAND(const int& clientSocket, const std::string& nickname, const std::string& cmd) {
	std::string	_fail = ":" + std::string(HOSTNAME) + " 441 " + nickname + " " + cmd + " :Unknown command\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRMOREPARAMS(const int& clientSocket, const std::string& nickname, const std::string& cmd) {
	std::string	_fail = ":" + std::string(HOSTNAME) + " 461 " + nickname + " " + cmd + " : Not enough parameters\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRINCORRECTPASSWORD(const int& clientSocket) {
	std::string	_fail = ":" + std::string(HOSTNAME) + " 464  :Password incorrect\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRNICKLENGTH(const std::string& nickname, const int& clientSocket) {
	std::string nickFail = ":" + std::string(HOSTNAME) + " 432 " + nickname + " :Erroneus nickname\r\n";
	send(clientSocket, nickFail.c_str(), nickFail.size(), 0);
}

void	ERRNICKNAMEINUSE(const std::string& nickname, const int& clientSocket) {
	std::string nickFail = ":" + std::string(HOSTNAME) + " 433 " + nickname + " :Nickname is already in use\r\n";
	send(clientSocket, nickFail.c_str(), nickFail.size(), 0);
}

void	WELCOME_001(const std::string& nickname, const int& clientSocket) {
	std::string welcome = "001 " + nickname + " :" MAGENTA "Welcome to Titi&Riri's IRC serv" EOC "\r\n";
	send(clientSocket, welcome.c_str(), welcome.length(), 0);
}

void	ERRNOSUCHCHANNEL(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = ":" + std::string(HOSTNAME) + " 403 " + nickname + " " + channelName + " :No such channel\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRNOTONCHANNEL(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = ":" + std::string(HOSTNAME) + " 442 " + nickname + " " + channelName + " :You're not on that channel\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRNOTINCHANNEL(const std::string& nickname, const std::string& other, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = ":" + std::string(HOSTNAME) + " 441 " + nickname + " " + other + " " + channelName + " :They're not on that channel\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRCLIENTUNKNOWN(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = ":" + std::string(HOSTNAME) + " 442 " + nickname + " " + channelName + " :Client just doesn't exist\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRINCORRECTKEY(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string _fail = ":" + std::string(HOSTNAME) + " 475 " + nickname + " " + channelName + " :Cannot join channel (+k) - incorrect key\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRUSERLIMIT(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string _fail = ":" + std::string(HOSTNAME) + " 471 " + nickname + " " + channelName + " :Cannot join channel (+l)\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRNOTOPERATOR(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = ":" + std::string(HOSTNAME) + " 482 " + nickname + " " + channelName + " :You're not on channel operator\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRINCHANNEL(const std::string& nickname, const std::string& guest, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = ":" + std::string(HOSTNAME) + " 443 " + nickname + " " + guest + " " + channelName + ":is already on channel\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRINVITEONLY(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string _fail = ":" + std::string(HOSTNAME) + " 473 " + nickname + " " + channelName + " :Cannot join channel (+i)\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	RPL_INVITING(const std::string& nickname, const std::string& guest, const std::string& channelName, const int& clientSocket) {
	std::string	inviteMessage = ":" + std::string(HOSTNAME) + " 341 " + nickname + " " + guest + " " + channelName + "\r\n";
	send(clientSocket, inviteMessage.c_str(), inviteMessage.length(), 0);
}

void	INVITE_MESSAGE(const std::string& guest, const std::string& channelName, const int& clientSocket) {
	std::string	inviteMessage = ":" + std::string(HOSTNAME) + " INVITE :" + channelName + " " + guest + "\r\n";
	send(clientSocket, inviteMessage.c_str(), inviteMessage.length(), 0);
}

void	RPL_NOTOPIC(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	(void)nickname;
	std::string	topicMessage = ":" + std::string(HOSTNAME) + " 331 " + channelName + " :No topic is set\r\n";
	send(clientSocket, topicMessage.c_str(), topicMessage.length(), 0);
}

void	RPL_TOPIC(const std::string& nickname, const std::string& channelName, const std::string& topic, const int& clientSocket) {
	std::string	topicMessage = ":" + std::string(HOSTNAME) + " 332 " + nickname + " " + channelName + " :" + topic + "\r\n";
	std::cout << "THE TOPIC : " << topic << std::endl;
	send(clientSocket, topicMessage.c_str(), topicMessage.length(), 0);
}
