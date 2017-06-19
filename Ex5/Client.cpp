//
// Created by yohai on 6/17/17.
//

#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <string>
#include <iostream>
#include "utilities.h"
#include "Client.h"

#define MAX_CLIENT_NAME 30
#define SEND_NAME(name) "cname " + name


int _cfd;
char _my_name[MAX_CLIENT_NAME +1];
struct sockaddr_in _sa;
struct hostent *hp;
fd_set server_set;

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
    _sa.sin_family = hp->h_addrtype;
    memcpy((char*)&_sa.sin_addr, hp->h_addr, hp->h_length);
    _sa.sin_family = hp->h_addrtype;
    _sa.sin_port = htons((u_short)port);

    if((_cfd = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0)
    {
        //TODO ERR
    }

    if(connect(_cfd, (struct sockaddr*)&_sa, sizeof(_sa)) < 0)
    {
        close(_cfd);
        //TODO err
    }
    FD_ZERO(&server_set);
    select(1, &server_set, NULL, NULL, NULL);//TODO put time out
    if(FD_ISSET(_cfd, &server_set))
    {
        //TODO check if can send something else than get name
        my_write(_cfd, SEND_NAME(std::string(name)));
    }

    FD_ZERO(&server_set);
    select(1, &server_set, NULL, NULL, NULL);//TODO put time out

    if(FD_ISSET(_cfd, &server_set))
    {
        char buf[943];
        my_read(_cfd, buf);
        std::cout << buf << std::endl;
        if(!strcmp(buf, CLIENT_CONECTED_SUCC))
        {
            close(_cfd);
            exit(1);
        }
    }
    return 0;
}

