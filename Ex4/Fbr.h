//
// Created by yohai on 5/26/17.
//

#ifndef EX4_FBR_H
#define EX4_FBR_H


#include <ctime>
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
    std::map<unsigned long, char*> _newMap, _middleMap;
    struct oldMap_cmp
    {
        bool operator()(const std::pair<unsigned long, unsigned int>& l, const std::pair<unsigned long, unsigned int>& r)
        {
            return l.first < r.first;
        }
    };
    std::map<std::pair<unsigned long, unsigned int>, char*, oldMap_cmp> _oldMap;


public:
    Fbr(int blocks_num, double f_old, double f_new);
    ~Fbr();
    char* get_next_block();
    void update_usage(char* it);
    bool cmp(const blc_data &a, const blc_data &b);
};


#endif //EX4_FBR_H
