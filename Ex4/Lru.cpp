//
// Created by yohai on 5/26/17.
//

#include "Lru.h"

Lru::Lru(int blocks_num) : CacheAlg(blocks_num)
{
    _usage_count = 0;
    _last_usage = new unsigned long[blocks_num];
    _blocks_num = blocks_num;
}

Lru::~Lru()
{
    delete[] _last_usage;
}

char* Lru::get_next_block()
{
    char* next_block;
    if ((_num_writen_blocks - _buf) / _block_size < _blocks_num)
    {
        next_block = _num_writen_blocks;
        _num_writen_blocks += _block_size;
    }
    else
    {
        next_block = _buf +
                ((std::min_element(_last_usage, _last_usage + _blocks_num) - _last_usage) * _block_size);
    }
//    update_usage(next_block);
    return next_block;
}

void Lru::update_usage(char *it)
{

    _last_usage[(it - _buf) / _block_size] = _usage_count;
    _usage_count++;
}

bool Lru::cmp(const blc_data &a, const blc_data &b)
{
    //TODO check if possible
    if(_last_usage[(std::get<2>(a) - _buf) / _block_size] == _last_usage[(std::get<2>(b) - _buf) / _block_size])
    {
        return std::get<2>(a) < std::get<2>(b);
    }
    return _last_usage[(std::get<2>(a) - _buf) / _block_size] > _last_usage[(std::get<2>(b) - _buf) / _block_size];
}

