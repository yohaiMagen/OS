//
// Created by yohai on 5/26/17.
//

#include "Lru.h"


Lru::Lru(int blocks_num) : CacheAlg(blocks_num)
{
    _last_usage = new std::time_t[blocks_num];
    _blocks_num = blocks_num;
}

Lru::~Lru()
{
    delete[] _last_usage;
}

char* Lru::get_next_block()
{
    char* next_block;
    if (_num_writen_blocks - _buf < _blocks_num)
    {
        next_block = _num_writen_blocks;
        _num_writen_blocks++;
    }
    else
    {
        next_block = _buf +
                     (std::min_element(_last_usage, _last_usage + _blocks_num) - _last_usage);
    }
    update_usage(next_block);
    return next_block;
}

void Lru::update_usage(char *it)
{
    _last_usage[it - _buf] = std::time(nullptr);
}

bool Lru::cmp(const lru_blc_data &a, const lru_blc_data &b)
{
    //TODO check if possible
    if(a.last_use == b.last_use)
    {
        return a.cache_p  < b.cache_p;
    }
    return a.last_use < b.last_use;
}

std::vector<blc_data&> Lru::sort_all_blocks()
{
    std::vector<blc_data&> all_blocks;
    for(auto it1 = _fd_allocator.begin(); it1 != _fd_allocator.end(); ++it1)
    {
        for(auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
        {
            lru_blc_data tmp;
            tmp.path = it1->first;
            tmp.blc_num_in_file = it2->first;
            tmp.cache_p = it2->second;
            tmp.last_use = _last_usage[it2->second - _buf];
            all_blocks.push_back(tmp);
        }
    }
    std::sort(all_blocks.begin(), all_blocks.end(), cmp);
    return all_blocks;
}

