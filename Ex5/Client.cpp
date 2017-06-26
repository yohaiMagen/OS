//
// Created by yohai on 6/17/17.
//

#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <regex>
#include <iostream>
#include <arpa/inet.h>
#include "utilities.h"

#define MAX_CLIENT_NAME 30
#define SEND_NAME(name) "cname " + name + "\n"


int _cfd;
char _my_name[MAX_CLIENT_NAME +1];
struct sockaddr_in _sa;
fd_set server_set, read_set;

int init(int port, char* ip, char* name)
{
    // init socket struct
    memset(&_sa, 0, sizeof(_sa));
    if(inet_aton( ip, &_sa.sin_addr) == 0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }
    _sa.sin_family = AF_INET;
    _sa.sin_port = htons((uint16_t)port);

    if((_cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        throw whatsapp_exeption(SYSTEM_ERR("socket"));
    }

    if(connect(_cfd, (struct sockaddr*)&_sa, sizeof(_sa)) < 0)
    {
        close(_cfd);
        throw whatsapp_exeption(SYSTEM_ERR("connect"));
    }
    FD_ZERO(&server_set);
    FD_SET(_cfd, &server_set);
    char buf[BUFF_SIZE];
    my_read(_cfd, buf);
    if (std::string(buf) == std::string(CLIENT_CONECTED_FAIL))
    {
        std::cout << CLIENT_CONECTED_FAIL;
        close(_cfd);
        exit(1);
    }
    if(FD_ISSET(_cfd, &server_set))
    {
        my_write(_cfd, SEND_NAME(std::string(name)));
    }

    FD_ZERO(&server_set);
    FD_SET(_cfd, &server_set);
    my_read(_cfd, buf);
    std::cout << buf;
    if(strcmp(buf, CLIENT_CONECTED_SUCC) != 0)
    {
        close(_cfd);
        exit(1);
    }
    FD_ZERO(&server_set);
    FD_SET(_cfd, &server_set);
    FD_SET(STDIN_FILENO, &server_set);
    return 0;
}

int user_input()
{
    char buf[BUFF_SIZE];
    my_read(STDIN_FILENO, buf);
    std::string input(buf);
    std::regex who(WHO);
    std::regex group("create_group [a-zA-Z0-9]+ ([a-zA-Z0-9]+,)*[a-zA-Z0-9]+");
    std::regex send("send [a-zA-Z0-9]+ .+");
    std::regex exit_rgx(EXIT);
    if(std::regex_match(input.substr(0, strlen(buf) - 1), who)||
            std::regex_match(input.substr(0, strlen(buf) - 1), group)||
            std::regex_match(input.substr(0, strlen(buf) - 1), send))
    {
        my_write(_cfd, input);
        my_read(_cfd, buf);
        std::cout << buf;

    }
    else if(std::regex_match(input.substr(0, strlen(buf) - 1), exit_rgx))
    {
        my_write(_cfd, input);
        my_read(_cfd, buf);
        std::cout << buf;
        close(_cfd);
        exit(0);
    }
    else
    {
        std::vector<std::string> split_msg;
        split(buf, split_msg, SPACE_CHAR, 3);
        if(split_msg[0] == CREATE_GROUP)
        {
            std::string group_name;
            try
            {
                group_name = split_msg.at(1);
            }
            catch(...)
            {
                group_name = "NONE";
            }

            std::cout << CREATE_GROUP_ERR(group_name);
        }
        else if(split_msg[0] ==  SEND)
        {
            std::cout << FAIL_SEND;
        }
        else if(split_msg[0] ==  WHO)
        {
            std::cout << FAIL_WHO;
        }
        else
        {
            std::cout << std::string(INVALID_INPUT);
        }
    }
    return 0;
}

int client_select()
{
    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO, &read_set);
    FD_SET(_cfd, &read_set);
    if(select(4, &read_set, NULL, NULL, NULL) < 0)
    {
        throw whatsapp_exeption(SYSTEM_ERR("select"));
    }
    if(FD_ISSET(STDIN_FILENO, &read_set))
    {
        user_input();
    }
    if(FD_ISSET(_cfd, &read_set))
    {
        char buf[BUFF_SIZE];
        my_read(_cfd, buf);
        if(std::string(buf) == std::string(SERVER_EXIT))
        {
            exit(1);
        }
        else
        {
            std::cout << buf;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        std::cerr << "Usage: whatsappClient clientName serverAddress serverPort" << std::endl;
        return 1;
    }
    try
    {
        init(atoi(argv[3]), argv[2], argv[1]);
        while (true)
        {
            client_select();
        }
    }
    catch(whatsapp_exeption e)
    {
        std::cerr << e.what();
        return 1;
    }
}

