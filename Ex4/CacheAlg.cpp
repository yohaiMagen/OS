//
// Created by yohai on 5/26/17.
//

#include <sys/stat.h>
#include <stdlib.h>
#include <new>
#include <fcntl.h>
#include <cstring>
#include "CacheAlg.h"


#define ERR -1;

CacheAlg::CacheAlg(int blocks_num)
{
    struct stat fi;
    stat("/tmp", &fi);
    int _block_size = fi.st_blksize;
    _buf_size = _block_size * blocks_num;
    if(_buf = (char*)aligned_alloc(_block_size, sizeof(char) * _buf_size))
    {
        throw std::bad_alloc();
    }
}

CacheAlg::~CacheAlg()
{
    free(_buf);
}

int CacheAlg::CacheFS_open(const char *pathname)
{
    if(strncmp("/tmp", pathname, 4))
    {
        return ERR;
    }
    int fd = open(pathname, O_RDONLY | O_DIRECT | O_SYNC);
    if(fd >= 0)
    {
        std::map<unsigned  int,unsigned  int> tmp;
        _fd_allocator[fd] = tmp;
    }
    return fd;




}