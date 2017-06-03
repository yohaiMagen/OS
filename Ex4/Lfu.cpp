#include <algorithm>
#include "Lfu.h"

Lfu::Lfu(int blocks_num) : CacheAlg(blocks_num)
{
    _num_usage = new unsigned int[blocks_num];
    _blocks_num = blocks_num;
}

Lfu::~Lfu()
{
    delete[] _num_usage;
}

char* Lfu::get_next_block()
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
                (std::min_element(_num_usage, _num_usage + _blocks_num) - _num_usage);
    }
    _num_usage[next_block - _buf] = 0;
    return next_block;
}

void Lfu::update_usage(char *it)
{
    _num_usage[it - _buf]++;
}

bool Lfu::cmp(const blc_data &a, const blc_data &b)
{
    if(_num_usage[std::get<3>(a) - _buf] == _num_usage[std::get<3>(b) - _buf])
    {
        return std::get<3>(a)  < std::get<3>(b);
    }
    return _num_usage[std::get<3>(a) - _buf] > _num_usage[std::get<3>(b) - _buf];
}