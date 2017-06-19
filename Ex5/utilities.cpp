//
// Created by nivkeren on 6/19/17.
//

#include "utilities.h"



int my_read(int fd, char* buf)
{
    char* buf_p = buf;
    int to_read = 943;
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
    return 0;
}

int my_write(int fd, std::string msg)
{
    char buf[msg.length() + 1];
    strcpy(buf, msg.c_str());
    char* buf_p = buf;
    int to_write = strlen(buf);
    int written = -1;
    while (to_write > 0 || written != 0)
    {

        if((written = read(fd, buf_p, to_write)) < 0)
        {
            //TODO ERR
        }
        buf_p += written;
        to_write -= written;
    }
    return 0;
}

void split(const std::string &s, std::vector<std::string>& result, char delim = SPACE_CHAR, unsigned int num_seg = -1)
{
    std::stringstream ss;
    ss.str(s);
    std::string item;
    if (num_seg < 0)
    {
        while (std::getline(ss, item, delim))
        {
            result.push_back(item);
        }
    }
    else
    {
        int pos = 0;
        for (unsigned int i = 0; i < num_seg; ++i)
        {
            int next = s.find(delim, pos);
            int len = 0;
            if (i == (num_seg - 1))
                len = s.length() - pos;
            else
            {
                len = next - pos;
            }
            result.push_back(s.substr(pos, len));
            pos = next + 1;
        }
    }
}