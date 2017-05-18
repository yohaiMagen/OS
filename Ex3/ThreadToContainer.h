//
// Created by yohai.magen on 5/9/17.
//

#ifndef EX3_THREADTOCONTAINER_H
#define EX3_THREADTOCONTAINER_H

#include <pthread.h>
#include "MapReduceClient.h"

typedef std::pair<k2Base*, v2Base*>  MAP_ITEM;
typedef std::vector<MAP_ITEM> MAP_ITEM_VEC;


struct ThreadToContainer
{
    pthread_t *_ithread;
    MAP_ITEM_VEC _container;
    int _read_pos;
    bool _thread_complete;
    pthread_mutex_t _cont_mutex;

    ThreadToContainer();
    ThreadToContainer(pthread_t *ithread, MAP_ITEM_VEC container, int read_pos, bool thread_done = false);
    ThreadToContainer(ThreadToContainer& other);
    ~ThreadToContainer();
    ThreadToContainer& operator=(const ThreadToContainer& other);


};


#endif //EX3_THREADTOCONTAINER_H
