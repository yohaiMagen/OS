//
// Created by yohai on 5/26/17.
//




#include "CacheAlg.h"


CacheAlg::CacheAlg(int blocks_num): _cash_hit(0), _cash_miss(0)
{
    // get OS block size
    struct stat fi;
    stat("/tmp", &fi);
    _block_size = fi.st_blksize;

    _buf_size = _block_size * blocks_num;
    _buf = (char*)aligned_alloc(_block_size, sizeof(char) * _buf_size);
    if(_buf == NULL)
    {
        throw std::bad_alloc();
    }
    real_block_size = new int[blocks_num];
    _num_writen_blocks = _buf;
}

CacheAlg::~CacheAlg()
{
    free(_buf);
    delete[] real_block_size;
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
    if(_fd2path.find(file_id) == _fd2path.end())
    {
        return ERR;
    }
    _fd2path.erase(file_id);
    return close(file_id);
}

int CacheAlg::CacheFS_pread(int file_id, void *buf, size_t count, off_t offset)
{
    /*
     * errors:
     * 1. buf is NULL
     * 2. negative offset
     * 3. unknown fd
     */
    if(buf == NULL || offset < 0 ||
       _fd2path.find(file_id) == _fd2path.end())
    {
        return ERR;
    }
    if(count == 0)
    {
        return count;
    }
    // final string and pointer
    char acc_str[count];
    char* acc_str_p = acc_str;
    // how many character read from buffer - real count
    int acc_str_size = 0;
    // blocks margins
    unsigned int first_blk = offset /_block_size;
    unsigned int last_blk = (offset + count - 1) /_block_size;

    for (unsigned int i = first_blk; i <= last_blk ; ++i)
    {
        // number of characters read from fd in the last chunk
        int read = -1;
        char* blc_start;
        // iterator to block in buffer
        auto it = _fd_allocator[_fd2path[file_id]].find(i);

        // block is not in buf
        if(it ==  _fd_allocator[_fd2path[file_id]].end())
        {
            blc_start = get_next_block();
            // translate to position on buf
            int buf_block_num = (blc_start - _buf) / _block_size;

            // fill the buffer
            int to_read = _block_size;
            read = pread(file_id, blc_start, (size_t)to_read, (i  * _block_size));
            to_read -= read;
            if(read != 0)
            {
                _cash_miss++;
                update_usage(blc_start);
                for (auto it1 = _fd_allocator.begin();
                     it1 != _fd_allocator.end(); ++it1)
                {
                    for (auto it2 = it1->second.begin();
                         it2 != it1->second.end(); ++it2)
                    {
                        if (it2->second == blc_start)
                        {
                            it1->second.erase(it2);
                        }
                    }
                }
                _fd_allocator[_fd2path[file_id]][i] = blc_start;
                // update real block size
                real_block_size[buf_block_num] = _block_size - to_read;
            }

            it = _fd_allocator[_fd2path[file_id]].find(i);
        }

        // block in buf
        else
        {
            _cash_hit++;
            blc_start = it->second;
            read = real_block_size[(blc_start - _buf) / _block_size];
            update_usage(blc_start);
        }

        char* off = blc_start;
        size_t n = read;
        //update margins to read from buf
        if ( i == first_blk)
        {
            off += offset % _block_size;
            if(i == last_blk)
            {
                if(n  > offset % _block_size)
                {
                    n = std::min(count, n - (offset % _block_size));
                }
                else
                {
                    n = 0;
                }

            }
            // get end of block
            else
            {
                n -= (offset % _block_size);
            }
        }
        else if(i == last_blk)
        {
            n = std::min((unsigned long)n, ((offset + count - 1) % _block_size) + 1);
        }

        // add to final string
        memcpy(acc_str_p, off, n);
        acc_str_p += n;
        acc_str_size += n;
        if(read == 0 || real_block_size[(blc_start-_buf)/_block_size] < (int)_block_size)
        {
            break;
        }
    }
    memcpy(buf, acc_str, acc_str_size);
    return acc_str_size;
}


int CacheAlg::CacheFS_print_cache(const char *log_path)
{
    std::ofstream slog = std::ofstream(log_path, std::ios_base::app);
    if(slog.fail())
    {
        return ERR;
    }
   blc_data_vec all_blocks;
    for(auto it1 = _fd_allocator.begin(); it1 != _fd_allocator.end(); ++it1)
    {
        for(auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
        {
            all_blocks.push_back(std::make_tuple(it1->first, it2->first, it2->second));
        }
    }
    all_blocks = merge_sort(all_blocks.begin(), all_blocks.end());
    for(auto it = all_blocks.begin(); it != all_blocks.end(); ++it)
    {
        slog << std::get<0>(*it) << " " <<  std::get<1>(*it) << std::endl;
        if(slog.fail())
        {
            return ERR;
        }
    }
    slog.close();
    return 0;
}

int CacheAlg::CacheFS_print_stat(const char *log_path)
{
    std::ofstream slog = std::ofstream(log_path, std::ios_base::app);
    if(slog.fail())
    {
        return ERR;
    }

    slog << STAT(_cash_hit, _cash_miss) << std::endl;
    if(slog.fail())
    {
        return ERR;
    }
    return 0;
}


blc_data_vec CacheAlg::merge_sort(blc_data_it begin, blc_data_it end)
{
    auto const size = std::distance(begin, end);
    if (size <= 1)
    {
        blc_data_vec tmp(begin, end);
        return tmp;
    }
    blc_data_it middle = begin;
    std::advance(middle, size / 2);

    blc_data_vec left = merge_sort(begin, middle);
    blc_data_vec right = merge_sort(middle, end);
    return merge(left, right);
}

blc_data_vec CacheAlg::merge(blc_data_vec& left, blc_data_vec& right)
{
    blc_data_vec tmp;

    blc_data_it l = left.begin();
    blc_data_it r = right.begin();
    while((l != left.end()) && (r != right.end()))
    {
         if(cmp(*l,*r))
         {
             tmp.push_back(*l);
             l++;
         }
         else
         {
             tmp.push_back(*r);
             r++;
         }
    }
    if(l == left.end())
    {
        tmp.insert(tmp.end(), r, right.end());
    }
    else
    {
        tmp.insert(tmp.end(), l, left.end());
    }
    return tmp;
}






