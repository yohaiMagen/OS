#ifndef EX4_FBR_H
#define EX4_FBR_H


#include <stdlib.h>
#include "CacheAlg.h"

/**
 * the position of the block in the old, middle, new sections
 */
enum POS
{
    OLD,
    NEW,
    MIDDLE,
    NONE //** initialisation value */
};

/**
 * frequancy Frequency-Based Replacement memory managent algorithm
 */
class Fbr: public CacheAlg
{
private:
    // an incremented variable to keep track of the number of times a block has been
    // read, used as a clock
    unsigned long long int _usage_count;
    // array of the last time a block has been accessed
    unsigned long long int* _last_usage;
    // array of number of times a block has been accessed
    unsigned int* _ref_count;
    // array of position in the buffer section of a block
    POS* _pos;
    // the numbers of blocks
    int _blocks_num;
    // size of each section
    unsigned int _old, _new, _middle;
    // a map of all blocks in each section for each section
    std::map<unsigned long, char*> _oldMap, _newMap, _middleMap;

public:
    Fbr(int blocks_num, double f_old, double f_new);
    ~Fbr();
    char* get_next_block();
    void update_usage(char* it);
    bool cmp(const blc_data &a, const blc_data &b);
};


#endif //EX4_FBR_H
