//
// Created by yohai on 5/26/17.
//




#include "Fbr.h"

Fbr::Fbr(int blocks_num, double f_old, double f_new):CacheAlg(blocks_num)
{
    _usage_count = 0;
    _new = f_new * blocks_num;
    _old = f_old * blocks_num;
    _middle = blocks_num - _new - _old;
    _blocks_num = blocks_num;
    _ref_count = new unsigned int[blocks_num];
    _last_usage = new unsigned long long int[blocks_num];
    _pos = new POS[blocks_num];
    for (int i = 0; i  < blocks_num ; ++i)
    {
        _pos[i] = NONE;
    }
}

Fbr::~Fbr()
{
    delete[] _ref_count;
    delete[] _last_usage;
    delete[] _pos;
}

char* Fbr::get_next_block()
{
    char* next_block;
    if ((_num_writen_blocks - _buf) / (int)_block_size < _blocks_num)
    {
        next_block = _num_writen_blocks;
        _num_writen_blocks += _block_size;
    }
    else
    {
        auto min_it = _oldMap.begin();
        for (auto it = _oldMap.begin(); it != _oldMap.end() ; ++it)
        {
            if(_ref_count[(it->second - _buf) / _block_size] <
                    _ref_count[(min_it->second - _buf) / _block_size])
            {
                min_it = it;
            }
        }
        next_block = min_it->second;
    }
    _ref_count[(next_block - _buf) / _block_size] = -1;
    return next_block;
}

void Fbr::update_usage(char *it)
{
    auto it_block = (it-_buf) / _block_size;

    // available free blocks
    if(_oldMap.size() < _old && _pos[it_block] == NONE)
    {
        if(_newMap.size() == _new)
        {
            auto map_it = _newMap.lower_bound(0);
            _middleMap[map_it->first] = map_it->second;
            _pos[(map_it->second- _buf) / _block_size] = MIDDLE;
            _newMap.erase(map_it);
            if(_middleMap.size() == _middle + 1)
            {
                map_it = _middleMap.lower_bound(0);
                _oldMap[map_it->first] = map_it->second;
                _pos[(map_it->second - _buf) / _block_size] = OLD;
                _middleMap.erase(map_it);
            }
       }

        _ref_count[it_block]++;
        _newMap[_usage_count] = it;
        _pos[it_block] = NEW;
    }
    else
    {
        auto map_it =_newMap.find(_last_usage[(it - _buf) / _block_size]);
        //deletes fro old or middle
        if(_pos[it_block] == NEW)
        {
            _newMap.erase(_last_usage[it_block]);
        }
        else
        {
            _ref_count[it_block]++;

            map_it = _newMap.lower_bound(0);
            _middleMap[map_it->first] = map_it->second;
            _pos[(map_it->second- _buf) / _block_size] = MIDDLE;
            _newMap.erase(map_it);

            if(_pos[it_block] != MIDDLE)
            {
                _oldMap.erase(_last_usage[it_block]);
                map_it = _middleMap.lower_bound(0);
                _oldMap[map_it->first] = map_it->second;
                _pos[(map_it->second - _buf) / _block_size] = OLD;
                _middleMap.erase(map_it);
            }
            else
            {
                _middleMap.erase(_last_usage[it_block]);
            }
        }
        _newMap[_usage_count] = it;
        _pos[(it - _buf) / _block_size] = NEW;
    }

    _last_usage[it_block] = _usage_count;
    _usage_count++;
}

bool Fbr::cmp(const blc_data &a, const blc_data &b)
{
    POS a_pos = _pos[(std::get<2>(a) - _buf) / _block_size];
    POS b_pos = _pos[(std::get<2>(b) - _buf) / _block_size];
    switch (a_pos)
    {
        case NEW:
            switch (b_pos)
            {
                case OLD:
                case MIDDLE:
                    return true;
                case NEW:
                    return _last_usage[(std::get<2>(a) - _buf) / _block_size] >=
                           _last_usage[(std::get<2>(b) - _buf) / _block_size];
                case NONE:
                    exit(ERR);
            }
        case MIDDLE:
            switch (b_pos)
            {
                case OLD:
                    return true;
                case MIDDLE:
                    return _last_usage[(std::get<2>(a) - _buf) / _block_size] >=
                           _last_usage[(std::get<2>(b) - _buf) / _block_size];
                case NEW:
                    return false;
                case NONE:
                    exit(ERR);
            }
        case OLD:
            switch (b_pos)
            {
                case OLD:
                    return _ref_count[(std::get<2>(a) - _buf) / _block_size] >=
                           _ref_count[(std::get<2>(b) - _buf) / _block_size];
                case MIDDLE:
                case NEW:
                    return false;
                case NONE:
                    exit(ERR);
            }
        default:
            exit(ERR);
    }
}



