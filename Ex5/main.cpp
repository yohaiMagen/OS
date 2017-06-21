


#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <regex>
#include "utilities.h"

int main(int argc, char *argv[])
{
    char c = ' ';
//    struct hostent *h;
//    if (argc != 2) {
//        fprintf(stderr, "usage: getip address\n");
//        exit(1);
//    }
//
//    if ((h=gethostbyname("localhost")) == NULL) {
//        fprintf(stderr, "gethostbyname ");
//        exit(1);
//    }
//    printf("Host name : %s\n", h->h_name);
//    printf("IP Address : %s\n",
//           inet_ntoa(*((struct in_addr *)h->h_addr)));
//    return 0;
    std::string s1("exit");
    std::string s12("who1");
    std::smatch t;
    std::regex regx("exit");
    if(std::regex_match(s1, regx))
    {
        printf("yaaaa");
    }

//    std::string s2("create_group group1 avi,beni,g0nv8dj,simon");
//    std::regex regx1("create_group [a-zA-Z0-9]+ ([a-zA-Z0-9]+,)*[a-zA-Z0-9]+");
//    if(std::regex_match(s2, regx1))
//    {
//        printf("yaaaa");
//    }



}