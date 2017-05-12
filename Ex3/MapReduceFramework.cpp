#include "ThreadToContainer.h"
#include <pthread.h>
#include "MapReduceFramework.h"
#include <algorithm>
#include <unordered_map>
#include <semaphore.h>
#include <map>
#include <fstream>
#include <ctime>

#define WORK_CHUNCK 5

//TODO make hidden
#define LOG_FILE_NAME "MapReduceFramework.log"
#define INTRO_MSG1 "RunMapReduceFramework started with "
#define INTRO_MSG2 " threads"
typedef std::pair<pthread_t*, OUT_ITEMS_VEC> REDUCE_THREAD;
typedef std::pair<k2Base*, V2_VEC> SHUFFLE_ITEM;

typedef std::map<k2Base*, V2_VEC> SHUFFLE_MAP;
typedef std::vector<SHUFFLE_ITEM> SHUFFLE_VEC;
typedef std::vector<ThreadToContainer*> MAP_THREAD_VEC;
typedef std::vector<REDUCE_THREAD> REDUCE_THREAD_VEC;

typedef struct MAP_INPUT
{
    IN_ITEMS_VEC *input_vec;
    MapReduceBase *mapReduceBase_object;
    int thread_num;
}MAP_INPUT;

MAP_THREAD_VEC pthread2Container;
REDUCE_THREAD_VEC reduce_thread2Container;
pthread_mutex_t pthread2Container_mutex;

pthread_t shuffle_th;
pthread_t *map_threads;
pthread_t *reduce_threads;

sem_t shuffle_read_sem;

pthread_mutex_t pos_map_mutex;
pthread_mutex_t pos_reduce_mutex;

SHUFFLE_VEC shuffle_prod_vec;

std::ofstream slog;
//----------------------------------------------------------------------------------------------------------------
OUT_ITEMS_VEC MergeReduceArray();
void shuffleMap2Vec(SHUFFLE_MAP &m, SHUFFLE_VEC &v);
void* f_map(void *arg);
void* f_shuffle(void *arg);
void* f_reduce(void *arg);
void new_thread_log(std::string);


OUT_ITEMS_VEC RunMapReduceFramework(MapReduceBase& mapReduce, IN_ITEMS_VEC& itemsVec,
                                    int multiThreadLevel, bool autoDeleteV2K2)
{

    slog = std::ofstream(LOG_FILE_NAME, std::ios_base::app);
    slog << "################################################\n##########################################\n###############################" << std::endl;//TODO remove
    slog << INTRO_MSG1 << multiThreadLevel << INTRO_MSG2 << std::endl;

    map_threads = new pthread_t[multiThreadLevel];
    reduce_threads = new pthread_t[multiThreadLevel];

    pos_map_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread2Container_mutex = PTHREAD_MUTEX_INITIALIZER;
    pos_reduce_mutex = PTHREAD_MUTEX_INITIALIZER;

    sem_init(&shuffle_read_sem, 0, 0);

    MAP_INPUT map_input;
    map_input.input_vec = &itemsVec;
    map_input.mapReduceBase_object = &mapReduce;

    pthread_mutex_lock(&pthread2Container_mutex);

    if(pthread_create(&shuffle_th, NULL, f_shuffle, &autoDeleteV2K2))
    {
        //TODO err
    }
    new_thread_log("shuffle");
    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        map_input.thread_num = i;
        if(pthread_create(&map_threads[i], NULL, f_map, (void*) &map_input))
        {
            //TODO err
        }
        new_thread_log("ExacMap");
        MAP_ITEM_VEC temp_vec;
        pthread2Container.push_back(new ThreadToContainer(&map_threads[i], temp_vec, 0, false));
    }

    pthread_mutex_unlock(&pthread2Container_mutex);
    for (int i = 0; i <  multiThreadLevel; ++i)
    {
        if(pthread_join(map_threads[i], NULL))
        {
            //TODO err
        }
    }
    sem_post(&shuffle_read_sem);

    if(pthread_mutex_destroy(&pos_map_mutex))
    {
        //TODO err
    }
    slog << "suff join" <<std::endl;
    pthread_join(shuffle_th, NULL);
    slog << "suff join pass" <<std::endl;
    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        if(pthread_create(&reduce_threads[i], NULL, f_reduce, &mapReduce))
        {
            //TODO err
        }
        new_thread_log("ExacReduce");
        OUT_ITEMS_VEC temp_vec;
        reduce_thread2Container.push_back(std::make_pair(&reduce_threads[i], temp_vec));
    }
    for (int j = 0; j < multiThreadLevel ; ++j)
    {
        pthread_join(reduce_threads[j], NULL);
    }
    if(pthread_mutex_destroy(&pos_reduce_mutex))
    {
        //TODO err
    }
    if(autoDeleteV2K2)
    {
        for (unsigned int i = 0; i < shuffle_prod_vec.size(); ++i)
        {
            for (unsigned int j = 0; j < shuffle_prod_vec[i].second.size(); ++j)
            {
                delete(shuffle_prod_vec[i].second[j]);
            }
            delete(shuffle_prod_vec[i].first);
        }
    }
    slog.flush();
    slog.close();

    delete[](map_threads);
    delete[](reduce_threads);

    return MergeReduceArray();
}

void* f_map(void *arg)
{
    slog<<"in map"<<std::endl;
    pthread_mutex_lock(&pthread2Container_mutex);
    pthread_mutex_unlock(&pthread2Container_mutex);

    static unsigned int pos = 0;
    MAP_INPUT *map_input = (MAP_INPUT*)arg;
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
        unsigned int start_point = pos;
        pos += WORK_CHUNCK;
        unsigned int end_point = pos;
        if(pthread_mutex_unlock(&pos_map_mutex))
        {
            //TODO err
        }
        if(end_point >= map_input->input_vec->size())
        {
            end_point = map_input->input_vec->size();
        }
        for (unsigned int i = start_point; i < end_point; ++i)
        {

            map_input->mapReduceBase_object->Map(map_input->input_vec->at(i).first, map_input->input_vec->at(i).second);
        }

    }
    for (unsigned int i = 0; i < pthread2Container.size(); ++i)
    {
        if(pthread_equal(*pthread2Container[i]->_ithread, pthread_self()))
        {
            pthread2Container[i]->_thread_done = true;
            break;
        }
    }
    slog <<" out map"<< std::endl;
    return NULL;
}

void* f_shuffle(void *arg)
{
    int getval = 0;
    slog << "in shf" << std::endl;
    bool autoDelete = *(bool*)arg;
    bool map_complete = true;
    while(map_complete)
    {
        sem_getvalue(&shuffle_read_sem, &getval);
        sem_wait(&shuffle_read_sem);
        sem_post(&shuffle_read_sem);
        sem_getvalue(&shuffle_read_sem, &getval);
        slog<<"pass sem" << std::endl;
        for (unsigned int i = 0; i < pthread2Container.size() ; ++i)
        {
            map_complete = map_complete && pthread2Container[i]->_thread_done;
            int dif  = pthread2Container[i]->_container.size() - pthread2Container[i]->_read_pos;
            if(dif > 0)
            {
                for (int j = pthread2Container[i]->_read_pos; j < pthread2Container[i]->_read_pos + dif ; ++j)
                {
//                    shuffle_prod[pthread2Container[i]->_container[j].first].push_back(pthread2Container[i]->_container[j].second);
                    bool found = false;
                    for (unsigned int k = 0; k < shuffle_prod_vec.size(); ++k)
                    {
                        k2Base* key = pthread2Container[i]->_container[j].first;
                        if (!(*key < *shuffle_prod_vec[k].first) &&
                            !(*shuffle_prod_vec[k].first < *key))
                        {
                            shuffle_prod_vec[k].second.push_back(pthread2Container[i]->_container[j].second);
                            if(autoDelete)
                            {
                                delete(key);
                            }
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                    {
                        V2_VEC temp;
                        v2Base *val = pthread2Container[i]->_container[j].second;
                        temp.push_back(val);
//                       temp.push_back(pthread2Container[i]->_container[j].second);
                        shuffle_prod_vec.push_back(std::make_pair(pthread2Container[i]->_container[j].first, temp));
                    }
                    sem_getvalue(&shuffle_read_sem, &getval);
                    sem_wait(&shuffle_read_sem);
                    sem_getvalue(&shuffle_read_sem, &getval);
                }
                pthread2Container[i]->_read_pos += dif;
            }
        }
        map_complete = !map_complete;
    }
    slog << "suff out" <<std::endl;
    return NULL;
}

void* f_reduce(void *arg)
{
    slog<<"in reduce"<<std::endl;
    static unsigned int pos = 0;
    MapReduceBase *mapReduceBase = (MapReduceBase*)arg;
    while(true)
    {
        if(pthread_mutex_lock(&pos_reduce_mutex))
        {
            //TODO err
        }
        if(pos >= shuffle_prod_vec.size())
        {
            if(pthread_mutex_unlock(&pos_reduce_mutex))
            {
                //TODO err
            }
            break;
        }
        unsigned int start_point = pos;
        pos += WORK_CHUNCK;
        unsigned int end_point = pos;
        if(pthread_mutex_unlock(&pos_reduce_mutex))
        {
            //TODO err
        }
        if(end_point >= shuffle_prod_vec.size())
        {
            end_point = shuffle_prod_vec.size();
        }
        for (unsigned int i = start_point; i < end_point; ++i)
        {

            mapReduceBase->Reduce(shuffle_prod_vec[i].first, shuffle_prod_vec[i].second);
        }
    }
    slog<<"reduce out"<<std::endl;
    return NULL;
}

void Emit2 (k2Base* key, v2Base* value)
{
    sem_post(&shuffle_read_sem);
    for (unsigned int i = 0; i < pthread2Container.size(); ++i)
    {
       if(pthread_equal(*pthread2Container[i]->_ithread, pthread_self()))
       {
           pthread2Container[i]->_container.push_back(std::make_pair(key, value));
           break;
       }
    }
}

void Emit3 (k3Base* key, v3Base* value)
{
    for (unsigned int i = 0; i < reduce_thread2Container.size(); ++i)
    {
        if(pthread_equal(*reduce_thread2Container[i].first, pthread_self()))
        {
            reduce_thread2Container[i].second.push_back(std::make_pair(key, value));
            break;
        }
    }
}

OUT_ITEMS_VEC MergeReduceArray()
{
    for (unsigned int i = 1; i < reduce_thread2Container.size(); ++i)
    {
        reduce_thread2Container[0].second.insert(reduce_thread2Container[0].second.end(),
                                                 reduce_thread2Container[i].second.begin(),
                                                 reduce_thread2Container[i].second.end());
    }
    std::sort(reduce_thread2Container[0].second.begin(), reduce_thread2Container[0].second.end());
    return reduce_thread2Container[0].second;
}

void shuffleMap2Vec(SHUFFLE_MAP &m, SHUFFLE_VEC &v)
{
    for (SHUFFLE_MAP::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        v.push_back(std::make_pair(it->first, it->second));
    }
}

void new_thread_log(std::string type)
{
    time_t now =  time(0);
    tm *ltm = localtime(&now);
    slog << "Thread " << type << " created [" <<
        ltm->tm_mday << "." << ltm->tm_mon + 1 << "." << ltm->tm_year + 1900 << " " <<
        ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "]" << std::endl;

}