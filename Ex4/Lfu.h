//
// Created by yohai on 5/26/17.
//

#ifndef EX4_LFU_H
#define EX4_LFU_H


#include "CacheAlg.h"

class Lfu: public CacheAlg
{


public:
    Lfu(int blocks_num);
    ~Lfu();

};


#endif //EX4_LFU_H
