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

bool Lru::cmp(const blc_data &a, const blc_data &b)
{
    //TODO check if possible
    if(_last_usage[std::get<3>(a) - _buf] == _last_usage[std::get<3>(b) - _buf])
    {
        return std::get<3>(a)  < std::get<3>(b);
    }
    return _last_usage[std::get<3>(a) - _buf] < _last_usage[std::get<3>(b) - _buf];
}

