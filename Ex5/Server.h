//
// Created by yohai on 6/17/17.
//

#ifndef EX5_SERVER_H
#define EX5_SERVER_H

#include <netinet/in.h>
#include <netdb.h>
#include <vector>

#define MAX_HOST_NAME 30

#define NUM_OF_CLIENTS 10

class Server
{

private:
    std::vector<int> clients;
    char _my_name[MAX_HOST_NAME + 1];
    int _sfd;
    int _port;
    struct sockaddr_in _sa;
    struct hostent *hp;


public:
    Server(int port);
    int accept_client();
    ~Server();

};


#endif //EX5_SERVER_H
