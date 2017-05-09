//
// Created by yohai on 5/6/17.
//

#include <pthread.h>
#include "MapReduceFramework.h"
#include <algorithm>
#include <unordered_map>
#include <semaphore.h>
#include <map>
#include "ThreadToContainer.h"

#define WORK_CHUNCK 5




typedef std::map<k2Base, std::vector<v2Base>> SHUFFLE_MAP;
typedef std::vector<ThreadToContainer> MAP_THREAD_VEC;

typedef struct MAP_INPUT
{
    IN_ITEMS_VEC *input_vec;
    MapReduceBase *mapReduceBase_object;
    int thread_num;
}MAP_INPUT;

MAP_THREAD_VEC pthread2Container;
pthread_mutex_t pthread2Container_mutex;

sem_t shuffle_read_sem;

MAP_ITEM_VEC map_prod[];
OUT_ITEMS_VEC reduce_prod[];


pthread_mutex_t pos_map_mutex;

SHUFFLE_MAP shuffle_prod;

//----------------------------------------------------------------------------------------------------------------
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
    pos_map_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread2Container_mutex = PTHREAD_MUTEX_INITIALIZER;
    sem_init(&shuffle_read_sem, 0, 0);
    MAP_INPUT map_input;
    map_input.input_vec = &itemsVec;
    map_input.mapReduceBase_object = &mapReduce;
    pthread_mutex_lock(&pthread2Container_mutex);
    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        map_input.thread_num = i;
        if(pthread_create(map_threads + i, NULL, f_map, (void*) &map_input))
        {
            //TODO err
        }
        MAP_ITEM_VEC temp_vec;
        pthread2Container.push_back(ThreadToContainer(map_threads + i, temp_vec, 0));
    }
    pthread_mutex_unlock(&pthread2Container_mutex);
    if(pthread_mutex_destroy(&pos_map_mutex))
    {
        //TODO err
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
        reduce_prod[0].insert(reduce_prod[0].end(), reduce_prod[i].begin(), reduce_prod[i].end());
    }
    std::sort(reduce_prod[0].begin(), reduce_prod[0].end());
    return reduce_prod[0];
}


void f_reduce()
{

}

void f_map(void *arg)
{
    pthread_mutex_lock(&pthread2Container_mutex);
    pthread_mutex_unlock(&pthread2Container_mutex);

    static int pos = 0;
    MAP_INPUT *map_input = dynamic_cast<MAP_INPUT*>(arg);
    while(true)
    {
        if(pthread_mutex_lock(&pos_map_mutex))
        {
            //TODO err
        }
        if(pos >= map_input->input_vec->size())
        {
            if(pthread_mutex_unlock(&pos_map_mutex))
            {
                //TODO err
            }
            break;
        }
        int start_point = pos;
        pos += WORK_CHUNCK;
        int end_point = pos;
        if(pthread_mutex_unlock(&pos_map_mutex))
        {
            //TODO err
        }
        if(end_point >= map_input->input_vec->size())
        {
            end_point = map_input->input_vec->size();
        }
        for (int i = start_point; i < end_point; ++i)
        {

            map_input->mapReduceBase_object->Map(map_input->input_vec->at(i).first, map_input->input_vec->at(i).second);
            sem_post(&shuffle_read_sem);

        }

    }
}

void shuffle()
{
    bool map_complete = true;
    while(map_complete)
    {
        sem_wait(&shuffle_read_sem);
        for (int i = 0; i < pthread2Container.size() ; ++i)
        {
            map_complete = map_complete && pthread2Container[i]._thread_done;
            int dif  = pthread2Container[i]._read_pos < pthread2Container[i]._container.size();
            if(dif > 0)
            {
                sem_post(&shuffle_read_sem);
                for (int j = pthread2Container[i]._read_pos; j < pthread2Container[i]._read_pos + dif ; ++j)
                {
                    shuffle_prod[*pthread2Container[i]._container[j].first].push_back(*pthread2Container[i]._container[j].second);
                    sem_wait(&shuffle_read_sem);
                }
                pthread2Container[i]._read_pos += dif;
            }
        }
        map_complete = !map_complete;
    }
    
}

void Emit2 (k2Base* key, v2Base* value)
{
    for (int i = 0; i < pthread2Container.size(); ++i)
    {
       if(pthread_equal(*pthread2Container[i]._ithread, pthread_self()))
       {
           pthread2Container[i]._container.push_back(std::make_pair(key, value));
           break;
       }
    }
}

void Emit3 (k3Base*, v3Base*)
{

}