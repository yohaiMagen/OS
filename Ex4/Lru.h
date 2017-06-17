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
    /*
     * represent the time in the life of that object
     */
    unsigned long long int _usage_count;
    // the time a block last been used
    unsigned long long int* _last_usage;
    // number of blocks in cache
    int _blocks_num;
public:
    /**
     * Lru constractor
     * @param blocks_num number of blocks
     */
    Lru(int blocks_num);
    /**
     * Lru destructor
     */
    ~Lru();
    /**
     * @see CasheAlg::get_next_block();
     *
     */
    char* get_next_block();
    /**
     * @see CasheAlg::update_usage();
     *
     */
    void update_usage(char* it);
    /**
     * @see CasheAlg::cmp();
     *
     */
    bool cmp(const blc_data &a, const blc_data &b);
};


#endif //EX4_LRU_H
