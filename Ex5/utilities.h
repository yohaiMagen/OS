//
// Created by nivkeren on 6/19/17.
//

#ifndef EX4_UTILITIES_H
#define EX4_UTILITIES_H

// -----------------------------------------------includes------------------------------------------------------------
#include <vector>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sstream>
#include "whatsapp_exception.h"
// -----------------------------------------------defines------------------------------------------------------------
#define BUFF_SIZE 1000

#define CREATE_GROUP "create_group"
#define SEND "send"
#define WHO "who"
#define EXIT "EXIT"
#define CLIENT_NAME "cname"

#define GET_NAME "whats your name\n"
#define SPACE_CHAR ' '

#define UN_REG "Unregistered successfully.\n"
#define WHO_MSG ": Requests the currently connected client names.\n"
#define FAIL_WHO "ERROR: failed to receive list of connected clients.\n"
#define SEND_MSG(msg, send_to)  ":\"" << msg << "\" was sent successfully to " << send_to << ".\n"
#define FAIL_SEND_SERVER(msg, send_to) "\"" << msg << "\" to " << send_to << ".\n"
#define FAIL_SEND "ERROR: failed to send.\n"
#define CLIENT_DELIM ','
#define SENT_SUCC "Sent successfully.\n"
#define CREATE_GROUP_ERR(group_name) "ERROR: failed to create group \"" + group_name + "\".\n"
#define CREATE_GROUP_SUCC(group_name) "Group \"" + group_name + "\" was created successfully.\n"
#define CLIENT_CONECTED_SUCC "Connected successfuly.\n"
#define CLIENT_CONECTED_FAIL "Failed to connect.\n"
#define CLIENT_CONECTED(name) name << " connected.\n"
#define ILLEGAL_REQUEST "illegal request.\n"
#define NAME_IN_USE "Client name is already in use.\n"
#define SERVER_EXIT "server_exit\n"
#define INVALID_INPUT "ERROR: Invalid input.\n"

// --------------------------------------------function deceleration----------------------------------------------------
/**
 * raed from the fd until reach the maximum length of a messge in the whatsapp app or until  end of a line
 * @param fd the file discriptur to read from
 * @param buf the buffer to write to
 * @return0 0 on success 1 othewise
 */
int my_read(int fd, char* buf);
/**
 * write all the contante of msg to the file discriptur fd
 * @param fd the file discriptur to write from
 * @param msg the messge to write
 * @return 0 on success 1 othewise
 */
int my_write(int fd, std::string msg);
/**
 * split the string s by the delimiter delim to num_seg segment or k if there is k delim and k < num_seg
 * put them in the vector result given by the caller
 * @param s the string to split
 * @param result a vector of splited strings
 * @param delim the delimiter to split by
 * @param num_seg the number of segment
 */
void split(const std::string &s, std::vector<std::string>& result, char delim = SPACE_CHAR, int num_seg = -1);

#endif //EX4_UTILITIES_H
