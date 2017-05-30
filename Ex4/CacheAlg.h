//
// Created by yohai on 5/26/17.
//

#ifndef EX4_CACHEALG_H
#define EX4_CACHEALG_H

#include <map>
#include <sys/stat.h>
#include <stdlib.h>
#include <new>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>

#define ERR -1;

class CacheAlg
{
protected:
    std::map<std::string , std::map<unsigned int, char*>> _fd_allocator;
//    std::map<std::string, unsigned int> _path2fd;
    std::map<int, std::string> _fd2path;
    int* real_block_size;
    char* _buf;
    char* _cur_blk;
    size_t _block_size;
    size_t _buf_size;
public:
    CacheAlg(int blocks_num );

    ~CacheAlg();

    int CacheFS_open(const char *pathname);

    int CacheFS_close(int file_id);

    int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset);

    virtual int CacheFS_print_cache (const char *log_path) = 0;

    virtual int CacheFS_print_stat (const char *log_path) = 0;

    virtual char* get_next_block() = 0;

    virtual void update_usage(char* it) = 0;
};




#endif //EX4_CACHEALG_H
