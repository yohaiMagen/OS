//
// Created by yohai on 5/26/17.
//

#ifndef EX4_CACHEALG_H
#define EX4_CACHEALG_H

#include <vector>
#include <algorithm>
#include <map>
#include <sys/stat.h>
#include <stdlib.h>
#include <new>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <fstream>
//returned when exit in error
#define ERR -1
//returned when exit in success
#define EXIT_SUCCESS 0

// macro for printing format of cache misses and hits
#define STAT(a ,b) "Hits number: " << a << "\nMisses number: " << b //<< "."

// tuple that contain all the data needed for a specific block
typedef std::tuple<std::string, int, char*> blc_data;
typedef std::vector<blc_data> blc_data_vec;
typedef std::vector<blc_data>::iterator blc_data_it;


class CacheAlg
{
private:
    /**
     * implimitation of merge sort that can sort withe the non static comperators cmp(not like std::sort)
     */
    blc_data_vec merge(blc_data_vec& left, blc_data_vec& right);
    blc_data_vec merge_sort(blc_data_it begin, blc_data_it end);

protected:
    /*
     * map between a path to a file and a map that map between the number of the block in the file
     * to a pointer to the block on the cache
     */
    std::map<std::string , std::map<unsigned int, char*>> _fd_allocator;
    /*
     * map between file descriptor to the path of the file
     */
    std::map<int, std::string> _fd2path;
    // an array of the real block size on the cache
    int* real_block_size;
    // pointer to the cache
    char* _buf;
    //pointer to the beginning of the cache for filling up the cache in the first time
    char* _num_writen_blocks;
    // block size of the running system
    size_t _block_size;
    // suze of the cache
    size_t _buf_size;
    // counter for the cache hit
    unsigned int _cash_hit;
    // counter for the cache miss
    unsigned int _cash_miss;
public:
/**
 *
 * @param blocks_num number of blocks in the cache
 * @return a cacheAlg object
 */
    CacheAlg(int blocks_num );
/**
 * CacheAlg destructor
 */
    virtual ~CacheAlg();
/**
 *
 * acording to CacheFS::CacheFS_open
 *
 */
    int CacheFS_open(const char *pathname);
/**
 *
 * acording to CacheFS::CacheFS_close
 *
 */
    int CacheFS_close(int file_id);
/**
 *
 * acording to CacheFS::CacheFS_pread
 *
 */
    int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset);
/**
 *
 * acording to CacheFS::CacheFS_print_cache
 *
 */
    int CacheFS_print_cache (const char *log_path);
/**
 *
 * acording to CacheFS::CacheFS_print_stat
 *
 */
    int CacheFS_print_stat (const char *log_path);
/**
 * get the next block from the cache to write to
 * @return a pointer to the next block in the cache
 */
    virtual char* get_next_block() = 0;
/**
 * update the usage of the block in the cache pointed by it
 * @param it pointer to a block in the cashe to update that it has been used
 */
    virtual void update_usage(char* it) = 0;
/**
 * compare tow blc-data and return the true if a will go out of the cache after b
 * @param a first block data
 * @param b second block data
 * @return true if a will go out of the cache before b
 */
    virtual bool cmp(const blc_data &a, const blc_data &b) = 0;
};




#endif //EX4_CACHEALG_H
