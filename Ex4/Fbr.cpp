//
// Created by yohai on 5/26/17.
//

#include "Fbr.h"

Fbr::Fbr(int blocks_num, double f_old, double f_new):CacheAlg(blocks_num)
{
    _new = f_new * blocks_num;
    _old = f_old * blocks_num;
    _middle = blocks_num - _new - _old;
    _blocks_num = blocks_num;
    _ref_count = new unsigned int[blocks_num];
    _last_usage = new time_t[blocks_num];
    _pos = new POS[blocks_num];

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
    if (_num_writen_blocks - _buf < _blocks_num)
    {
        next_block = _num_writen_blocks;
        _num_writen_blocks++;
    }
    else
    {

        next_block = _oldMap.lower_bound(0)->second;
    }
    _ref_count[next_block - _buf] = 0;
    return next_block;
}

void Fbr::update_usage(char *it)
{
    std::time_t c_time = std::time(nullptr);
    if(_oldMap.size() < _old)
    {
        _oldMap[c_time] = it;
        _pos[it-_buf] = OLD;
    }
    else if(_middleMap.size() < _middle)
    {
        _middleMap[c_time] = it;
        _pos[it-_buf] = MIDDLE;
    }
    else if(_newMap.size() < _new)
    {
        _newMap[c_time] = it;
        _pos[it-_buf] = NEW;
    }
    else
    {
        auto map_it =_newMap.find(_last_usage[it - _buf]);
        //deletes fro old or middle
        if(_pos[it-_buf] == NEW)
        {
            _newMap.erase(_last_usage[it-_buf]);
        }
        else
        {
            _ref_count[it-_buf]++;

            map_it = _newMap.lower_bound(0);
            _middleMap[map_it->first] = map_it->second;
            _pos[map_it->second- _buf] = MIDDLE;
            _newMap.erase(map_it);

            if(_pos[it-_buf] != MIDDLE)
            {
                _oldMap.erase(_last_usage[it-_buf]);
                map_it = _middleMap.lower_bound(0);
                _oldMap[map_it->first] = map_it->second;
                _pos[map_it->second - _buf] = OLD;
                _middleMap.erase(map_it);
            }
            else
            {
                _middleMap.erase(_last_usage[it-_buf]);
            }
        }
        _newMap[c_time] = it;
        _pos[it - _buf] = NEW;
    }

    _last_usage[it-_buf] = c_time;

}

bool Fbr::cmp(const blc_data &a, const blc_data &b)
{
    POS a_pos = _pos[std::get<3>(a) - _buf];
    POS b_pos = _pos[std::get<3>(b) - _buf];
    switch (a_pos)
    {
        case NEW:
            switch (b_pos)
            {
                case OLD:
                case MIDDLE:
                    return true;
                case NEW:
                    return _last_usage[std::get<3>(a) - _buf] >= _last_usage[std::get<3>(b) - _buf];
            }
        case MIDDLE:
            switch (b_pos)
            {
                case OLD:
                    return true;
                case MIDDLE:
                    return _last_usage[std::get<3>(a) - _buf] >= _last_usage[std::get<3>(b) - _buf];
                case NEW:
                    return false;
            }
        case OLD:
            switch (b_pos)
            {
                case OLD:
                    return _ref_count[std::get<3>(a) - _buf] >= _ref_count[std::get<3>(b) - _buf];
                case MIDDLE:
                case NEW:
                    return false;
            }
    }
}

