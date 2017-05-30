//
// Created by yohai on 5/26/17.
//

#ifndef EX4_LRU_H
#define EX4_LRU_H


#include "CacheAlg.h"

class Lru: public CacheAlg
{
private:
    std::map<char*,timeval> _least_recent;
public:
    Lru(int blocks_num);
    ~Lru();
    char* get_next_block();

};


#endif //EX4_LRU_H
