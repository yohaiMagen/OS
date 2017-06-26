#include <netinet/in.h>
#include <netdb.h>
#include <vector>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <unistd.h>
#include <set>
#include <map>
#include <iostream>
#include <arpa/inet.h>
#include <limits>
#include <stdlib.h>
#include "utilities.h"

// --------------------------------- defines ---------------------------------
#define MAX_HOST_NAME 30

#define NUM_OF_CLIENTS 30

// number of clients the server listen to at once
#define CLIENT_PER_SOCKET 10

// -------------------------functions deceleration---------------------------------
int illegal_request(int fd);
int init(int port);
int accept_client();
int terminate_server(int status);
int server_std_input();
int cname(std::string name, int fd);
int create_group(std::string group_name, std::string client_list, int fd);
int send_msg(std::string send_to, std::string msg, int fd);
int who(int fd);
void terminate_client(int fd);
int client_operation(int fd);
int illegal_request(int fd);
int server_select();

char _my_name[MAX_HOST_NAME + 1];
//server fd
int _sfd;
struct sockaddr_in _sa;
struct hostent *hp;

fd_set clientsfds, readfds;
std::unordered_map<int, std::string> fd2name;
std::map<std::string, int> name2fd;
std::unordered_map<std::string, std::set<int>> groups;

/**
 * initialize the server
 * @param port - int, the number of port
 * @return 0 if initialized successfully, -1 if an error accord
 */
int init(int port)
{
    if((_sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        throw whatsapp_exeption(SYSTEM_ERR("socket"));
    }

    //hostnet initilization
    gethostname(_my_name, MAX_HOST_NAME);
    hp = gethostbyname(_my_name);
    if (hp == NULL)
    {
        throw whatsapp_exeption(SYSTEM_ERR("gethostbyname"));
    }


    // init socket struct
    memset(&_sa, 0, sizeof(struct sockaddr_in));
    _sa.sin_family = (sa_family_t)hp->h_addrtype;
    memcpy(&_sa.sin_addr, hp->h_addr, (int)hp->h_length);
    _sa.sin_port = htons((uint16_t)port);
    if (bind(_sfd , (struct sockaddr *)&_sa , sizeof(struct
            sockaddr_in)) < 0)
    {
        close(_sfd);
        throw whatsapp_exeption(SYSTEM_ERR("bind"));
    }

    listen(_sfd, CLIENT_PER_SOCKET);
    FD_ZERO(&clientsfds);
    FD_SET(_sfd, &clientsfds);
    FD_SET(STDIN_FILENO, &clientsfds);
    std::cin.clear();
    return 0;
}

/**
 * accept the client to the server
 * @return 0 if  successful, -1 if failed
 */
int accept_client()
{

    int client_fd;
    if((client_fd = accept(_sfd, NULL, NULL)) < 0)
    {
        throw whatsapp_exeption(SYSTEM_ERR("accept"));
    }
    if (fd2name.size() > NUM_OF_CLIENTS)
    {
        my_write(client_fd, CLIENT_CONECTED_FAIL);
        close(client_fd);
        return -1;
    }
    else
    {
        my_write(client_fd, GET_NAME);
        FD_SET(client_fd, &clientsfds);
        fd2name[client_fd] = "NONE";
    }
    return 0;
}

/**
 * termintes the server and all the clients that are connected to the server
 * @param status the reason for termination
 *               0 if terminated intentionally, 1 if the server is terminated because of
 *               an error
 * @return 0 if  successful, -1 if failed
 */
int terminate_server(int status)
{
    for (auto it = fd2name.begin(); it != fd2name.end(); ++it)
    {
        if(close(it->first))
        {
            throw whatsapp_exeption(SYSTEM_ERR("close"));
        }
    }
    if(close(_sfd))
    {
        throw whatsapp_exeption(SYSTEM_ERR("close"));
    }
    exit(status);
}

/**
 * reads from the servers standard input and compiles the command accordingly
 * @return 0 if  successful, -1 if failed
 */
int server_std_input()
{
    char buf[BUFF_SIZE];
    my_read(STDIN_FILENO, buf);
    if(strcmp(buf, "EXIT/n"))
    {
        for(auto it =  fd2name.begin(); it != fd2name.end(); ++it)
        {
            my_write(it->first, SERVER_EXIT);
        }
        terminate_server(0);
    }
    else
    {
        std::cout << INVALID_INPUT;
    }
    return 0;
}

/**
 * handles a name that is sent by a client
 * @param name string, the name of the new client
 * @param fd int, the number of the new clients fd
 * @return 0 if successful, -1 if failed
 */
int cname(std::string name, int fd)
{
    if(name2fd.find(name) != name2fd.end() ||
            groups.find(name) != groups.end())
    {
        my_write(fd,NAME_IN_USE);
        fd2name.erase(fd);
        FD_CLR(fd, &clientsfds);
        close(fd);
        return -1;
    }
    fd2name[fd] = name;
    name2fd[name] = fd;
    std::cout << CLIENT_CONECTED(name);
    my_write(fd, CLIENT_CONECTED_SUCC);

    return 0;
}

/**
 * create a new group of clients
 * @param group_name string, the name of the new group
 * @param client_list string, the list of clients
 * @param fd int, the fd of the group creator
 * @return 0 if  successful, -1 if failed
 */
int create_group(std::string group_name, std::string client_list, int fd)
{
    if(groups.find(group_name) != groups.end() || name2fd.find(group_name) != name2fd.end())
    {
        my_write(fd, CREATE_GROUP_ERR(group_name));
        std::cout << fd2name[fd] << ": " << CREATE_GROUP_ERR(group_name);
        return -1;
    }
    std::vector<std::string> split_client_list;
    split(client_list, split_client_list, CLIENT_DELIM);
    std::set<int> client_fds;
    for(auto it = split_client_list.begin(); it != split_client_list.end(); ++it)
    {
        if(name2fd.find(*it) != name2fd.end())
        {
            client_fds.insert(name2fd[*it]);
        }
        else
        {
            my_write(fd, CREATE_GROUP_ERR(group_name));
            std::cout << fd2name[fd] << ": " << CREATE_GROUP_ERR(group_name) ;
            return -1;
        }
    }
    //insert the creator client
    client_fds.insert(fd);
    if(client_fds.size() == 1)
    {
        my_write(fd, CREATE_GROUP_ERR(group_name));
        std::cout << fd2name[fd] << ": " << CREATE_GROUP_ERR(group_name);
        return -1;
    }
    groups[group_name] = client_fds;
    std::cout << fd2name[fd] << ": " << CREATE_GROUP_SUCC(group_name);
    my_write(fd, CREATE_GROUP_SUCC(group_name));
    return 0;
}

/**
 * sends the given message to the given client(recipient)
 * @param send_to string, the name of the recipient client
 * @param msg string, the message to be send
 * @param fd int, the fd of the sender
 * @return 0 if  successful, -1 if failed
 */
int send_msg(std::string send_to, std::string msg, int fd)
{
    std::string msg_to_client = fd2name[fd] + ":" + msg + "\n";
    if(name2fd.find(send_to) != name2fd.end())
    {
        my_write(name2fd[send_to], msg_to_client );
    }
    else if(groups.find(send_to) != groups.end() &&
            groups[send_to].find(fd) != groups[send_to].end())
    {
        for(auto it = groups[send_to]. begin(); it != groups[send_to].end(); ++it)
        {
            if(*it != fd)
            {
                my_write(*it, msg_to_client);
            }
        }
    }
    else
    {
        my_write(fd, FAIL_SEND);
        std::cout << fd2name[fd] << FAIL_SEND_SERVER(msg, send_to);
        return -1;
    }
    my_write(fd, SENT_SUCC);
    std::cout << fd2name[fd] << SEND_MSG(msg, send_to);
    return 0;
}

/**
 * send the lisat of users in alphabetical order to the given fd.
 * @param fd int, the fd of the client that asked the question
 * @return 0 if  successful, -1 if failed
 */
int who(int fd)
{
    auto it = name2fd.begin();
    std::string str = it->first;
    ++it;
    for(; it != name2fd.end(); ++it)
    {
        str = str + "," + it->first;
    }
    str = str + ".\n";
    my_write(fd, str);
    std::cout << fd2name[fd] << WHO_MSG;
    return 0;
}

/**
 * terminates and deletes the client from the server
 * @param fd int, the fd of the terminated client
 */
void terminate_client(int fd)
{
    for(auto it = groups.begin(); it != groups.end(); ++it)
    {
        it->second.erase(fd);
    }
    std::string name = fd2name[fd];
    fd2name.erase(fd);
    name2fd.erase(name);
    my_write(fd, UN_REG);
    FD_CLR(fd, &clientsfds);
    close(fd);
    std::cout << name << ":" << UN_REG;

}

/**
 * parses and compiles the message sent from the client to the server
 * @param fd int, the fd of the client connected to the server
 * @return 0 if  successful, -1 if failed
 */
int client_operation(int fd)
{
    char buf[BUFF_SIZE];
    my_read(fd, buf);
    std::vector<std::string> split_msg;
    split(buf, split_msg, SPACE_CHAR, 3);
    if(split_msg[0] == CLIENT_NAME)
    {
        cname(split_msg[1], fd);
    }
    else if(split_msg[0] == CREATE_GROUP)
    {
        create_group(split_msg[1], split_msg[2], fd);
    }
    else if(split_msg[0] ==  SEND)
    {
        send_msg(split_msg[1], split_msg[2], fd);
    }
    else if(split_msg[0] ==  WHO)
    {
        who(fd);
    }
    else if(split_msg[0] ==  EXIT)
    {
        terminate_client(fd);
    }
    else
    {
        illegal_request(fd);
    }
    return 0;
}

/**
 * send the given fd a fail message
 * @param fd int, client's fd
 * @return 0 if  successful, -1 if failed
 */
int illegal_request(int fd)
{
    return my_write(fd, ILLEGAL_REQUEST);
}

/**
 * server's main function. gets the ready to read from fd and acts accordingly
 * @return
 */
int server_select()
{
    readfds = clientsfds;
    if(select(CLIENT_PER_SOCKET +1, &readfds, NULL, NULL, NULL) < 0)
    {
        throw whatsapp_exeption(SYSTEM_ERR("select"));
    }

    if( FD_ISSET(STDIN_FILENO, &readfds))
    {
        server_std_input();
    }
    if (FD_ISSET(_sfd, &readfds))
    {
        accept_client();
    }
    for(auto it = fd2name.begin(); it != fd2name.end(); ++it)
    {
        if(FD_ISSET(it->first, &readfds))
        {
            client_operation(it->first);
            break;
        }
    }
    return 0;

}

/**
 * main
 */
int main(int argc, char **argv)
{

    if(argc != 2)
    {
        std::cerr << "“Usage: whatsappServer portNum" << std::endl;
        return 1;
    }
    try
    {
        init(std::stoi(argv[1]));
        while (true)
        {
            server_select();
        }
    }
    catch(whatsapp_exeption e)
    {
        std::cerr << e.what();
        return 1;
    }
}
