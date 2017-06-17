//
// Created by yohai on 6/17/17.
//

#include <cstring>
#include <zconf.h>
#include "Server.h"


#define CLIENT_PER_SOCET 5

Server::Server(int port): _port(port)
{
    if((_sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        //TODO ERR
    }

    //hostnet initilization
    gethostname(_my_name, MAX_HOST_NAME);
    hp = gethostbyname(_my_name);
    if (hp == NULL)
    {
        //TODO ERR
    }


    // init socket struct
    memset(&_sa, 0, sizeof(struct sockaddr_in));
    _sa.sin_family = hp->h_addrtype;
    memcpy(&_sa.sin_addr, hp->h_addr, hp->h_length);
    _sa.sin_port = htons(_port);

    if (bind(_sfd , (struct sockaddr *)&_sa , sizeof(struct
            sockaddr_in)) < 0)
    {
        close(_sfd);
        //TODO ERR
    }

    listen(_sfd, CLIENT_PER_SOCET);
}

int Server::accept_client()
{
    int client_fd;
    if((client_fd = accept(_sfd, NULL, NULL)) < 0)
    {
        return -1;
    }
    clients.push_back(client_fd);
}
