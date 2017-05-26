//
// Created by yohai on 5/26/17.
//

#ifndef EX4_CACHEALG_H
#define EX4_CACHEALG_H

#include <map>

class CacheAlg
{
private:
    std::map<int, std::map<unsigned int, unsigned int>> _fd_allocator;
    char* _buf;
    size_t _block_size;
    size_t _buf_size;
public:
    CacheAlg(int blocks_num );

    ~CacheAlg();

    int CacheFS_open(const char *pathname);

    virtual int CacheFS_close(int file_id) = 0;

    virtual int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset) = 0;

    virtual int CacheFS_print_cache (const char *log_path) = 0;

    virtual int CacheFS_print_stat (const char *log_path) = 0;
};




#endif //EX4_CACHEALG_H
