//
// Created by yohai on 5/6/17.
//

#include <pthread.h>
#include "MapReduceFramework.h"

typedef std::pair<k2Base, v2Base>  MAP_ITEM;
typedef std::pair<k2Base, std::vector<v2Base>>  SHUFFLE_ITEM;

typedef std::vector<MAP_ITEM> MAP_ITEM_VEC;
typedef std::vector<SHUFFLE_ITEM> SHUFFLE_ITEM_VEC;

MAP_ITEM_VEC map_prod[];
OUT_ITEMS_VEC reduce_prod[];
SHUFFLE_ITEM shuffle_prod;

void MergeVecArray2Vec();

OUT_ITEMS_VEC MergeReduceArray();

OUT_ITEMS_VEC RunMapReduceFramework(MapReduceBase& mapReduce, IN_ITEMS_VEC& itemsVec,
                                    int multiThreadLevel, bool autoDeleteV2K2)
{
    map_prod = new MAP_ITEM_VEC[multiThreadLevel];
    reduce_prod = new OUT_ITEMS_VEC[multiThreadLevel];
    pthread_t shuffle_th;

    if(pthread_create(&shuffle_th, NULL, shuffle, NULL))
    {
        //TODO err
    }
    pthread_t map_threads[multiThreadLevel];
    pthread_t reduce_threads[multiThreadLevel];
    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        if(pthread_create(map_threads + i, NULL, f_map, (void*) &itemsVec))
        {
            //TODO err
        }
    }
    pthread_join(shuffle_th, NULL);
    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        if(pthread_create(reduce_threads + i, NULL, f_reduce, NULL))
        {
            //TODO err
        }
    }
    for (int j = 0; j < multiThreadLevel ; ++j)
    {
        pthread_join(reduce_threads[j], NULL);
    }
    return MergeReduceArray();
}

OUT_ITEMS_VEC MergeReduceArray()
{
    for (int i = 1; i < reduce_prod->size(); ++i)
    {
        reduce_prod[0].insert(reduce_prod[0].end, reduce_prod[i].begin(), reduce_prod[i].end());
    }
    return reduce_prod[0];
}


void f_reduce()
{

}

void f_map(void *arg)
{

}

void shuffle()
{

}

