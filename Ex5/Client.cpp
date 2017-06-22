//
// Created by yohai on 6/17/17.
//

#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <regex>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include "utilities.h"
#include "Client.h"

#define MAX_CLIENT_NAME 30
#define SEND_NAME(name) "cname " + name + "\n"


int _cfd;
char _my_name[MAX_CLIENT_NAME +1];
struct sockaddr_in _sa;
struct hostent *hp;
fd_set server_set, read_set;

int init(int port, char* ip, char* name)
{

    //hostnet initilization
    hp = gethostbyname(ip);
    if (hp == NULL)
    {
        //TODO ERR
    }


    // init socket struct
    memset(&_sa, 0, sizeof(_sa));
//    memcpy((char*)&_sa.sin_addr, inet_addr(ip), strlen(ip));
    if(inet_aton( ip, &_sa.sin_addr) == 0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }
    _sa.sin_family = AF_INET;
    _sa.sin_port = htons(port);

    if((_cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        //TODO ERR
    }

    if(connect(_cfd, (struct sockaddr*)&_sa, sizeof(_sa)) < 0)
    {
        close(_cfd);
        exit(1);
        //TODO err
    }
    FD_ZERO(&server_set);
    FD_SET(_cfd, &server_set);
//    select(2, &server_set, NULL, NULL, NULL);//TODO put time out
    char buf[BUFF_SIZE];
    my_read(_cfd, buf);
    if(FD_ISSET(_cfd, &server_set))
    {
        //TODO check if can send something else than get name
        my_write(_cfd, SEND_NAME(std::string(name)));
    }

    FD_ZERO(&server_set);
    FD_SET(_cfd, &server_set);
//    select(1, &server_set, NULL, NULL, NULL);//TODO put time out
//    if(FD_ISSET(_cfd, &server_set))
//    {
        my_read(_cfd, buf);
        std::cout << buf;
        if(strcmp(buf, CLIENT_CONECTED_SUCC) != 0)
        {
            close(_cfd);
            exit(1);
        }
//    }
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
    std::regex who("who");
    std::regex group("create_group [a-zA-Z0-9]+ ([a-zA-Z0-9]+,)*[a-zA-Z0-9]+");
    std::regex send("send [a-zA-Z0-9]+ .+");
    std::regex exit_rgx("exit");
    if(std::regex_match(input.substr(0, strlen(buf) - 1), who)||
            std::regex_match(input.substr(0, strlen(buf) - 1), group)||
            std::regex_match(input.substr(0, strlen(buf) - 1), send)||
            std::regex_match(input.substr(0, strlen(buf) - 1), exit_rgx))
    {
        my_write(_cfd, input);
        my_read(_cfd, buf);
        std::cout << buf;
        if(std::regex_match(input.substr(0, strlen(buf) - 1), exit_rgx))
        {
            exit(0);
        }
    }
    else
    {
        //TODO command err
    }

}

int client_select()
{
    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO, &read_set);
    FD_SET(_cfd, &read_set);
    if(select(11, &read_set, NULL, NULL, NULL) < 0)
    {
        //TODO ERR
    }
    if(FD_ISSET(STDIN_FILENO, &read_set))
    {
        user_input();
    }
    if(FD_ISSET(_cfd, &read_set))
    {
        char buf[BUFF_SIZE];
        my_read(_cfd, buf);
        std::cout << buf;
    }

}

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        std::cerr << "invalid input" << std::endl;
    }
    init(atoi(argv[3]), argv[2], argv[1]);
    while (true)
    {
        client_select();
    }
    return 0;
}

