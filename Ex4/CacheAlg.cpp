//
// Created by yohai on 5/26/17.
//


#include "CacheAlg.h"




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
    real_block_size = new int[blocks_num];
    _cur_blk = _buf;
}

CacheAlg::~CacheAlg()
{
    free(_buf);
    delete[](real_block_size);
}

int CacheAlg::CacheFS_open(const char *pathname)
{
    int fd =  -1;
    if(strncmp("/tmp", pathname, 4))
    {
        return ERR;
    }
    fd = open(pathname, O_RDONLY | O_DIRECT | O_SYNC);
    if (fd >= 0)
    {
        if(_fd_allocator.find(pathname) == _fd_allocator.end())
        {
            std::map<unsigned int, char *> tmp;
            _fd_allocator[pathname] = tmp;
        }
        _fd2path[fd] = pathname;
    }
    return fd;
}

int CacheAlg::CacheFS_close(int file_id)
{
    //TODO use iterator in erase to improve complexity
    if(_fd2path.find(file_id) == _fd2path.end())
    {
        return ERR;
    }
    _fd2path.erase(file_id);
    return close(file_id);
}

int CacheAlg::CacheFS_pread(int file_id, void *buf, size_t count, off_t offset)
{
    //TODO cache hit n miss
    if(buf == NULL || offset < 0 ||
       _fd2path.find(file_id) == _fd2path.end())
    {
        return ERR;
    }
    char acc_str[count];
    char* acc_str_p = acc_str;
    int actual_cnt = 0;
    int first_blk = offset/_block_size;
    int last_blk = (offset+count)/_block_size;
    for (int i = first_blk; i < last_blk ; ++i)
    {
        int read = -1;
        auto it = _fd_allocator[_fd2path[file_id]].find(i);
        if(it ==  _fd_allocator[_fd2path[file_id]].end())
        {
            char* next = get_next_block();
            int buf_blok_num =(next-_buf)/_block_size;
            int to_read = _block_size;

            while(to_read > 0 || read != 0)
            {
                read = pread(file_id, next, _block_size, i * _block_size);
                if(read < 0)
                {
                    return ERR;
                }
                next = next + read;
                to_read = to_read - read;
            }
            real_block_size[buf_blok_num] = _block_size-to_read;
        }
        char* off = it->second;
        size_t n = real_block_size[(it->second-_buf)/_block_size];
        if ( i == first_blk)
        {
            off += offset % _block_size;
            if(i == last_blk)
            {
                n = std::min(count, n-(offset % _block_size));
            }
            else
            {
                n -= (offset % _block_size);
            }
        }
        else if(i == last_blk)
        {
            int n = std::min(n, (offset + count)%_block_size);
        }
        memcpy(acc_str_p, off, n);
        acc_str_p += n;
        actual_cnt += n;
        if(read == 0 || real_block_size[(it->second-_buf)/_block_size] < _block_size)
        {
            break;
        }
        update_usage(it->second);
    }
    memcpy(buf, acc_str, actual_cnt);
    return actual_cnt;
}




