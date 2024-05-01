/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 18:13:58 by rchbouki          #+#    #+#             */
/*   Updated: 2024/05/01 17:46:01 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcUtils.hpp"

void	ERRUNKNOWNCOMMAND(const int& clientSocket, const std::string& nickname, const std::string& cmd) {
	std::string	_fail = ":localhost 441 " + nickname + " " + cmd + " :Unknown command\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRMOREPARAMS(const int& clientSocket, const std::string& nickname, const std::string& cmd) {
	std::string	_fail = ":localhost 461 " + nickname + " " + cmd + " : Not enough parameters\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRINCORRECTPASSWORD(const int& clientSocket) {
	std::string	_fail = ":localhost 464  :Password incorrect\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	WELCOME_001(const std::string& nickname, const int& clientSocket) {
	std::string welcome = "001 " + nickname + " :" MAGENTA "Welcome to Titi&Riri's IRC serv" EOC "\r\n";
	send(clientSocket, welcome.c_str(), welcome.length(), 0);
}

void	ERRNOSUCHCHANNEL(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = "localhost 403 " + nickname + " " + channelName + " :No such channel\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRNOTONCHANNEL(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = "localhost 442 " + nickname + " " + channelName + " :You're not on that channel\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRNOTINCHANNEL(const std::string& nickname, const std::string& other, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = "localhost 441 " + nickname + " " + other + " " + channelName + " :They're not on that channel\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRCLIENTUNKNOWN(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = "localhost 442 " + nickname + " " + channelName + " :Client just doesn't exist\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRINCORRECTKEY(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string _fail = ":localhost 475 " + nickname + " " + channelName + " :Cannot join channel (+k) - incorrect key\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRUSERLIMIT(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string _fail = ":localhost 471 " + nickname + " " + channelName + " :Cannot join channel (+l)\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRNOTOPERATOR(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = "localhost 482 " + nickname + " " + channelName + " :You're not on channel operator\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRINCHANNEL(const std::string& nickname, const std::string& guest, const std::string& channelName, const int& clientSocket) {
	std::string	_fail = "localhost 443 " + nickname + " " + guest + " " + channelName + ":is already on channel\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	ERRINVITEONLY(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	std::string _fail = ":localhost 473 " + nickname + " " + channelName + " :Cannot join channel (+i)\r\n";
	send(clientSocket, _fail.c_str(), _fail.size(), 0);
}

void	RPL_INVITING(const std::string& nickname, const std::string& guest, const std::string& channelName, const int& clientSocket) {
	std::string	inviteMessage = ":localhost 341 " + nickname + " " + guest + " " + channelName + "\r\n";
	send(clientSocket, inviteMessage.c_str(), inviteMessage.length(), 0);
}

void	INVITE_MESSAGE(const std::string& nickname, const std::string& guest, const std::string& channelName, const int& clientSocket) {
	std::string	inviteMessage = ":" + nickname + "!~user@host INVITE :" + channelName + " " + guest + "\r\n";
	send(clientSocket, inviteMessage.c_str(), inviteMessage.length(), 0);
}

void	RPL_NOTOPIC(const std::string& nickname, const std::string& channelName, const int& clientSocket) {
	(void)nickname;
	std::string	topicMessage = ":localhost 331 " + channelName + " :No topic is set\r\n";
	send(clientSocket, topicMessage.c_str(), topicMessage.length(), 0);
}

void	RPL_TOPIC(const std::string& nickname, const std::string& channelName, const std::string& topic, const int& clientSocket) {
	std::string	topicMessage = ":localhost 332 " + nickname + " " + channelName + " :" + topic + "\r\n";
	std::cout << "THE TOPIC : " << topic << std::endl;
	send(clientSocket, topicMessage.c_str(), topicMessage.length(), 0);
}
