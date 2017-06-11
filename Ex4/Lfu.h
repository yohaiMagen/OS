#ifndef EX4_LFU_H
#define EX4_LFU_H

#include "CacheAlg.h"
struct blc_data
{
    std::string path;
    int blc_num_in_file;
    char* cache_p;
    int ref_count;
};

class Lfu: public CacheAlg
{

private:
    static unsigned int* _ref_count;
    int _blocks_num;

public:

    std::vector<blc_data> sort_all_blocks();

    Lfu(int blocks_num);
    ~Lfu();
    char* get_next_block();
    void update_usage(char* it);
    static bool cmp(const blc_data &a, const blc_data &b);
    int CacheFS_print_cache(const char *log_path);


};
#endif //EX4_LFU_H
