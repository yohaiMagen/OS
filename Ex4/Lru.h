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
    static std::time_t* _last_usage;
    int _blocks_num;
    struct lru_blc_data : public blc_data
    {
        std::time_t last_use;
    };
public:
    Lru(int blocks_num);
    ~Lru();
    char* get_next_block();
    void update_usage(char* it);
    bool cmp(const lru_blc_data &a, const lru_blc_data &b);
    std::vector<blc_data*> sort_all_blocks();
};


#endif //EX4_LRU_H
