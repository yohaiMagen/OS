//
// Created by yohai on 5/26/17.
//

#ifndef EX4_FBR_H
#define EX4_FBR_H


#include <stdlib.h>
#include "CacheAlg.h"

enum POS
{
    OLD,
    NEW,
    MIDDLE,
    NONE
};

class Fbr: public CacheAlg
{
private:
    unsigned long _usage_count;
    unsigned long* _last_usage;
    unsigned int* _ref_count;
    POS* _pos;
    int _blocks_num;
    unsigned int _old, _new, _middle;
    std::map<unsigned long, char*> _oldMap, _newMap, _middleMap;


public:
    Fbr(int blocks_num, double f_old, double f_new);
    ~Fbr();
    char* get_next_block();
    void update_usage(char* it);
    bool cmp(const blc_data &a, const blc_data &b);
};


#endif //EX4_FBR_H
