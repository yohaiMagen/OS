//
// Created by yohai on 6/17/17.
//


#include <netinet/in.h>
#include <netdb.h>
#include <vector>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <unordered_map>
#include <unistd.h>


#define MAX_HOST_NAME 30

#define NUM_OF_CLIENTS 10

#define CLIENT_PER_SOCET 10

#define SPACE_CHAR ' '


#define CREATE_GROUP "create_group"
#define SEND "send"
#define WHO "who\n"
#define EXIT "exit\n"
#define CLIENT_NAME "cname"



std::vector<int> clients;
char _my_name[MAX_HOST_NAME + 1];
int _sfd;
int _port;
struct sockaddr_in _sa;
struct hostent *hp;
fd_set clientsfds, readfds;
std::unordered_map<int, std::string> fd2name;



int init(int port)
{
    if((_sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        //TODO ERR
    }

    _port = port;

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
    FD_ZERO(&clientsfds);
    FD_SET(_sfd, &clientsfds);
    FD_SET(STDIN_FILENO, &clientsfds);
}

int accept_client()
{
    int client_fd;
    if((client_fd = accept(_sfd, NULL, NULL)) < 0)
    {
        return -1;
    }
    clients.push_back(client_fd);
    FD_SET(client_fd, &clientsfds);
    return 0;
}

int readFromfd(int fd, char* buf)
{
    char* buf_p = buf;
    int to_read = 256;
    int read_T = -1;
    while (to_read > 0 || read_T != 0)
    {

        if((read_T = read(fd, buf_p, to_read)) < 0)
        {
            //TODO ERR
        }
        buf_p += read_T;
        to_read -= read_T;

    }
}

int terminateServer(int status)
{
    for (int i = 0; i < clientsfds.fd_count ; ++i)
    {
        if(close(clientsfds.fd_array[i]))
        {
            //TODO ERR
        }
    }
    if(close(_sfd))
    {
        //TODO ERR
    }
    exit(status);
}

int serverStdInput()
{
    char buf[256];
    readFromfd(STDIN_FILENO, buf);
    if(strcmp(buf, "EXIT/n"))
    {
        terminateServer(0);
    }
    else
    {
        //TODO ERR
    }
}
void split(const std::string &s, std::vector<std::string>& result, unsigned int num_seg = -1)
{
    std::stringstream ss;
    ss.str(s);
    std::string item;
    if (num_seg < 0) {
        while (std::getline(ss, item, SPACE_CHAR)) {
            result.push_back(item);
        }
    }
    else
    {
        int pos = 0;
        for (unsigned int i = 0; i < num_seg; ++i)
        {
            int next = s.find(' ', pos);
            int len = 0;
            if ( i == (num_seg - 1))
                len = s.length() - pos;
            else
            {
                len = next - pos;
            }
            result.push_back(s.substr(pos, len));
            pos = next + 1;
    }
}

int cname(std::string name, int fd)
{
    fd2name[fd] = name;
    return 0;
}

int client_operation(int fd)
{
    char buf[256];
    readFromfd(fd, buf);
    std::vector<std::string> split_msg;
    split(buf, split_msg, 3);
    if(split_msg[0] == CLIENT_NAME)
    {
        int x = cname(split_msg[1], fd);
    }
    else if(split_msg[0] == CREATE_GROUP)
    {
        create_group(split_msg[1], split_msg[2]);
    }
    else if(split_msg[0] ==  SEND)
    {
        send_msg(split_msg[1], split_msg[2]);
    }
    else if(split_msg[0] ==  WHO)
    {
        who();
    }
    else if(split_msg[0] ==  EXIT)
    {
        terminate_client(fd);
    }
    else
    {
        // TODO ERR
    }

}

int select_client()
{
    readfds = clientsfds;
    if(select(CLIENT_PER_SOCET +1, &readfds, NULL, NULL, NULL) < 0)//TODO check if select need time limit
    {
        //TODO err
    }

    if( FD_ISSET(STDIN_FILENO, &readfds))
    {
        serverStdInput();
    }
    if (FD_ISSET(_sfd, &readfds))
    {
        accept_client();
    }
    FD_CLR(_sfd, &readfds);
    FD_CLR(STDIN_FILENO, &readfds);
    for (int i = 0; i < readfds.fd_count ; ++i)
    {
        client_operation(readfds.fd_array[i]);


    }
    return 0;

}

int main()
{
    init();
    while (true)
    {
        select_client();
    }

}
