#ifndef EX4_LFU_H
#define EX4_LFU_H

#include "CacheAlg.h"

class Lfu: public CacheAlg
{

private:
    unsigned int _ref_count[];
    int _blocks_num;
public:
    Lfu(int blocks_num);
    ~Lfu();
    char* get_next_block();
    void update_usage(char* it);
    bool cmp(const blc_data &a, const blc_data &b);

};
#endif //EX4_LFU_H
