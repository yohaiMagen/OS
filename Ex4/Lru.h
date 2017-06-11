//
// Created by yohai on 5/26/17.
//

#ifndef EX4_LRU_H
#define EX4_LRU_H


#include <ctime>
#include "CacheAlg.h"

class Lru: public CacheAlg
{
private:
    unsigned long _usage_count;
    unsigned long* _last_usage;
    int _blocks_num;
public:
    Lru(int blocks_num);
    ~Lru();
    char* get_next_block();
    void update_usage(char* it);
    bool cmp(const blc_data &a, const blc_data &b);
};


#endif //EX4_LRU_H
