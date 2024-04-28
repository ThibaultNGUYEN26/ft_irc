/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilityFunctions.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 18:07:57 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/28 20:01:00 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sys/socket.h>

#define EOC "\033[1;1;0m"
#define MAGENTA "\033[1;35m"

void	ERRINCORRECTPASSWORD(const int& clientSocket);
void	WELCOME_001(const std::string& nickname, const int& clientSocket);
void	ERRNOSUCHCHANNEL(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRNOTONCHANNEL(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRCLIENTUNKNOWN(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRINCORRECTKEY(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRUSERLIMIT(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRNOTOPERATOR(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	ERRINCHANNEL(const std::string& nickname, const std::string& guest, const std::string& channelName, const int& clientSocket);
void	ERRINVITEONLY(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	RPL_INVITING(const std::string& nickname, const std::string& guest, const std::string& channelName, const int& clientSocket);
void	INVITE_MESSAGE(const std::string& nickname, const std::string& guest, const std::string& channelName, const int& clientSocket);
void	RPL_NOTOPIC(const std::string& nickname, const std::string& channelName, const int& clientSocket);
void	RPL_TOPIC(const std::string& nickname, const std::string& channelName, const std::string& topic, const int& clientSocket);
