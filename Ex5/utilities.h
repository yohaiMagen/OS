//
// Created by nivkeren on 6/19/17.
//

#ifndef EX4_UTILITIES_H
#define EX4_UTILITIES_H


#include <vector>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sstream>

#define CREATE_GROUP "create_group"
#define SEND "send"
#define WHO "who\n"
#define EXIT "exit\n"
#define CLIENT_NAME "cname"

#define GET_NAME "whats your name"
#define SPACE_CHAR ' '

#define UN_REG "Unregistered successfully."
#define WHO_MSG ": Requests the currently connected client names."
#define SEND_MSG(msg, send_to)  ":\"" << msg << "\" was sent successfully to" << send_to << "."
#define FAIL_SEND_SERVER(msg, send_to) "\"" << msg << "\" to " << send_to << "."
#define FAIL_SEND "ERROR: failed to send."
#define CLIENT_DELIM ','
#define SENT_SUCC "Sent successfully."
#define CREATE_GROUP_ERR(group_name) "ERROR: failed to create group \"" + group_name + "\"."
#define CREATE_GROUP_SUCC(group_name) "Group \"" + group_name + "\' was created successfully."
#define CLIENT_CONECTED_SUCC "Connected successfuly."
#define CLIENT_CONECTED(name) name << "connected."

int my_read(int fd, char* buf);

int my_write(int fd, std::string msg);

void split(const std::string &s, std::vector<std::string>& result, char delim = SPACE_CHAR, unsigned int num_seg = -1)

#endif //EX4_UTILITIES_H
