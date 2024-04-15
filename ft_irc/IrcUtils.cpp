/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 18:03:13 by rchbouki          #+#    #+#             */
/*   Updated: 2024/04/15 23:47:06 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

bool	isValidNickname(const std::string& nickname, clientMap& clients) {
	if (nickname.length() > 9) {
		std::cout << RED "Nickname's length exceeds 9 characters. Change it before registering again." EOC << std::endl;	
		return false;
	}
	if (clients.find(nickname) != clients.end()) {
		std::cout << RED "Nickname already exists within the server's clients. Change it before registering again." EOC << std::endl;
		return false;
	}
	return true;
}

void	handleJoinCommand(int clientSocket, const std::string& channelName, clientMap& clients, channelMap& channels) {
	// Check if the channel exists, create it if not
	std::map<std::string, std::vector<int> >::iterator it = channels.find(channelName);
	if (it == channels.end()) {
		std::vector<int> clientSockets;
		clientSockets.push_back(clientSocket);
		channels[channelName] = clientSockets;
	} else {
		it->second.push_back(clientSocket);
	}
	// Notify all clients in the channel about the new member
	std::string nickname;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			nickname = (it->second)->getNickname();
			break;
		}
	}
	// Send JOIN message back to the client to confirm
	std::string joinConfirm = ":" + nickname + "!~user@host JOIN :" + channelName + "\r\n";
	send(clientSocket, joinConfirm.c_str(), joinConfirm.size(), 0);

	std::vector<int>& members = channels[channelName];
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != clientSocket) {
			send(*memberIt, joinConfirm.c_str(), joinConfirm.size(), 0);
		}
	}
}

void	handleLeaveCommand(int clientSocket, const std::string& channelName, clientMap& clients, channelMap& channels) {
	// Check if the channel exists
	std::map<std::string, std::vector<int> >::iterator it = channels.find(channelName);
	if (it != channels.end()) {
		// Find the client's socket in the channel members list and remove it
		std::vector<int>& members = it->second;
		std::vector<int>::iterator memberIt = members.begin();
		while (memberIt != members.end()) {
			if (*memberIt == clientSocket) {
				memberIt = members.erase(memberIt);
			} else {
				++memberIt;
			}
		}

		// If the channel becomes empty, remove it from the channels map
		if (members.empty()) {
			channels.erase(it);
		}
	}

	// Notify the client about leaving the channel
	std::string nickname;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			nickname = (it->second)->getNickname();
			break;
		}
	}

	// Send PART message back to the client to confirm leaving
	std::string leaveConfirm = ":" + nickname + "!~user@host PART :" + channelName + "\r\n";
	send(clientSocket, leaveConfirm.c_str(), leaveConfirm.size(), 0);

	// Notify all clients in the channel about the member leaving
	std::vector<int>& members = channels[channelName];
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != clientSocket) {
			send(*memberIt, leaveConfirm.c_str(), leaveConfirm.size(), 0);
		}
	}
}

void handleKickCommand(int clientSocket, const std::string& channelName, const std::string& userToKick, const std::string& reason, clientMap& clients, channelMap& channels) {
    // Check if the channel exists
    channelMap::iterator channelIt = channels.find(channelName);
    if (channelIt == channels.end()) {
        std::cerr << "No such channel: " << channelName << std::endl;
        return; // No such channel
    }

    // Find the user to kick based on their nickname
    int userSocket = -1;
    for (clientMap::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getNickname() == userToKick) {
            userSocket = it->second->getSocket();
            break;
        }
    }

    if (userSocket == -1) {
        std::cerr << "User not found: " << userToKick << std::endl;
        return; // User not found in server
    }

    // Check if the user is in the channel
    std::vector<int>::iterator pos = std::find(channelIt->second.begin(), channelIt->second.end(), userSocket);
    if (pos == channelIt->second.end()) {
        std::cerr << "User not found in channel: " << userToKick << std::endl;
        return; // User not in channel
    }

    // Remove the user from the channel
    channelIt->second.erase(pos);


	std::string nickname;
	for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((it->second)->getSocket() == clientSocket) {
			nickname = (it->second)->getNickname();
			break;
		}
	}
    // Notify the kicked user and all channel members
    std::string kickMessage = ":" + nickname + "!~user@host KICK " + channelName + " " + userToKick + " :" + (reason.empty() ? "No reason" : reason) + "\r\n";

    // Send message to all clients in the channel and the kicked user
    for (size_t i = 0; i < channelIt->second.size(); ++i) {
        send(channelIt->second[i], kickMessage.c_str(), kickMessage.length(), 0);
    }
    send(userSocket, kickMessage.c_str(), kickMessage.length(), 0);  // Ensure the kicked user also receives the message
}

void handleTopicCommand(int clientSocket, const std::string& channelName, const std::string& newTopic, clientMap& clients, channelMap& channels) {
    // Check if the channel exists
    channelMap::iterator channelIt = channels.find(channelName);
    if (channelIt == channels.end()) {
        std::cerr << "No such channel: " << channelName << std::endl;
        return; // No such channel
    }

    // Check if a new topic is provided
    if (!newTopic.empty()) {
        // Check if the client sending the command is in the channel
        std::vector<int>& members = channelIt->second;
        bool clientInChannel = false;
        for (size_t i = 0; i < members.size(); ++i) {
            if (members[i] == clientSocket) {
                clientInChannel = true;
                break;
            }
        }
        if (!clientInChannel) {
            std::cerr << "Client not in channel: " << channelName << std::endl;
            return; // Client not in channel
        }

        // Update the topic of the channel
        // Note: In this simple example, assuming the topic is stored as part of the channel data structure.
        // You should adjust this according to your actual implementation.
        // For example, if the topic is stored in a separate class, adjust accordingly.
        //channelIt->second.setTopic(newTopic);

        // Notify all clients in the channel about the new topic
        std::string nickname;
        for (clientMap::iterator it = clients.begin(); it != clients.end(); it++) {
            if ((it->second)->getSocket() == clientSocket) {
                nickname = (it->second)->getNickname();
                break;
            }
        }

        std::string topicMessage = ":" + nickname + "!~user@host TOPIC " + channelName + " :" + newTopic + "\r\n";
        for (size_t i = 0; i < members.size(); ++i) {
            send(members[i], topicMessage.c_str(), topicMessage.length(), 0);
        }
    } else {
        // If no new topic is provided, send the current topic back to the client
        const std::string& currentTopic = "Example topic"; // Modify this line to retrieve the actual topic from your data structure
        if (currentTopic.empty()) {
            std::string noTopicMessage = "There is no topic set for " + channelName + "\r\n";
            send(clientSocket, noTopicMessage.c_str(), noTopicMessage.length(), 0);
        } else {
            std::string currentTopicMessage = "Current topic for " + channelName + " is: " + currentTopic + "\r\n";
            send(clientSocket, currentTopicMessage.c_str(), currentTopicMessage.length(), 0);
        }
    }
}

void	broadcastToChannel(int senderSocket, const std::string& channelName, const std::string& message, clientMap& clients, channelMap& channels) {
	// Iterate over clients to find the nickname of the sender.
	std::string nickname;
	for (std::map<std::string, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->getSocket() == senderSocket) {
			nickname = it->second->getNickname();
			break;
		}
	}
	// Find the channel in the map
	std::map<std::string, std::vector<int> >::iterator channelIt = channels.find(channelName);
	if (channelIt == channels.end()) {
		std::string fullMessage = nickname + " " + channelName + " :" + "No such channel" + "\r\n";
		send(senderSocket, fullMessage.c_str(), fullMessage.length(), 0);
		return ;
	}
	// Construct the message using the sender's nickname.
	std::string fullMessage = ":" + nickname + "!~user@host PRIVMSG " + channelName + " :" + message + "\r\n";

	// Broadcast the message to all channel members except the sender.
	std::vector<int>& members = channelIt->second;
	for (std::vector<int>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		if (*memberIt != senderSocket) {
			send(*memberIt, fullMessage.c_str(), fullMessage.length(), 0);
		}
	}
}

void	sendDM(int senderSocket, const std::string& target, const std::string& message, clientMap& clients) {
	// Check if the target exists
 	std::map<std::string, Client*>::iterator it = clients.find(target);
	int	targetSocket;
	if (it == clients.end()) {
		std::string fullMessage = target + " : There was no such nickname\r\n";
		send(senderSocket, fullMessage.c_str(), fullMessage.length(), 0);
		return ;
	}
	else {
		targetSocket = clients[target]->getSocket();
	}
	// retrieve nickname of the sender
	std::string nickname;
	for (std::map<std::string, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->getSocket() == senderSocket) {
			nickname = it->second->getNickname();
			break;
		}
	}
	std::string fullMessage = ":" + nickname + " PRIVMSG " + target + " :" + message + "\r\n";
	std::cout << fullMessage << std::endl;
	send(targetSocket, fullMessage.c_str(), fullMessage.length(), 0);
}
