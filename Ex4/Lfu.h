#ifndef EX4_LFU_H
#define EX4_LFU_H

#include "CacheAlg.h"

class Lfu: public CacheAlg
{

private:
    // _ref_count[i] save the times that the i'th block in the caache
    unsigned int* _ref_count;
    // number of blocks in cache
    int _blocks_num;
public:
    /**
    * Lru constractor
    * @param blocks_num number of blocks
    */
    Lfu(int blocks_num);
    /**
     * Lfu destructor
     */
    ~Lfu();
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
#endif //EX4_LFU_H
