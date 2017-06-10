#include <algorithm>
#include "Lfu.h"

Lfu::Lfu(int blocks_num) : CacheAlg(blocks_num)
{
    _ref_count = new unsigned int[blocks_num];
    _blocks_num = blocks_num;
}

Lfu::~Lfu()
{
    delete[] _ref_count;
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
                (std::min_element(_ref_count, _ref_count + _blocks_num) - _ref_count);
    }
    _ref_count[next_block - _buf] = 0;
    return next_block;
}

void Lfu::update_usage(char *it)
{
    _ref_count[it - _buf]++;
}

bool Lfu::cmp(const blc_data &a, const blc_data &b)
{
    if(_ref_count[std::get<3>(a) - _buf] == _ref_count[std::get<3>(b) - _buf])
    {
        return std::get<3>(a)  < std::get<3>(b);
    }
    return _ref_count[std::get<3>(a) - _buf] > _ref_count[std::get<3>(b) - _buf];
}