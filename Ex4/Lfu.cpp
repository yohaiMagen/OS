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
    if(a.ref_count == b.ref_count)
    {
        return a.cache_p < b.cache_p;
    }
    return a.ref_count > b.ref_count;
}

std::vector<blc_data> Lfu::sort_all_blocks()
{
    std::vector<blc_data> all_blocks;
    for(auto it1 = _fd_allocator.begin(); it1 != _fd_allocator.end(); ++it1)
    {
        for(auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
        {
            blc_data tmp;
            tmp.path = it1->first;
            tmp.blc_num_in_file = it2->first;
            tmp.cache_p = it2->second;
            tmp.ref_count = _ref_count[it2->second - _buf];
            all_blocks.push_back(tmp);
        }
    }
    std::sort(all_blocks.begin(), all_blocks.end(), cmp);
    return all_blocks;
}

int Lfu::CacheFS_print_cache(const char *log_path)
{
    std::ofstream slog = std::ofstream(log_path, std::ios_base::app);
    if(slog.fail())
    {
        return ERR;
    }

    std::vector<blc_data> all_blocks = sort_all_blocks();
    for(auto it = all_blocks.begin(); it != all_blocks.end(); ++it)
    {

        slog << it->path << " " <<  it->blc_num_in_file<< std::endl;
        if(slog.fail())
        {
            return ERR;
        }
    }
    slog.close();
    return 0;
}