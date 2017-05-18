#include "ThreadToContainer.h"
#include <pthread.h>
#include "MapReduceFramework.h"
#include <algorithm>
#include <unordered_map>
#include <semaphore.h>
#include <map>
#include <fstream>
#include <ctime>
#include <iostream>


#define WORK_CHUNCK 10

// *********************************** log messages *************************************
//TODO make hidden
#define LOG_FILE_NAME "MapReduceFramework.log"
#define INTRO_MSG1 "RunMapReduceFramework started with "
#define INTRO_MSG2 " threads"

#define SHUFFLE_THREAD_NAME "shuffle"
#define MAP_THREAD_NAME "ExacMap"
#define REDUCE_THREAD_NAME "ExacReduce"

// *********************************** typedefs *****************************************
// container of thread and its output container (for shuffle thread) 
typedef std::pair<pthread_t*, OUT_ITEMS_VEC> REDUCE_THREAD;
// output item of the shuffle procedure
typedef std::pair<k2Base*, V2_VEC> SHUFFLE_ITEM;

// container of shuffle procedure product 
typedef std::map<k2Base*, V2_VEC> SHUFFLE_MAP;
// shuffle output (container of shuffle items)
typedef std::vector<SHUFFLE_ITEM> SHUFFLE_VEC;

// container of map threads, it's output container and more features 
typedef std::vector<ThreadToContainer*> MAP_THREAD_VEC;

// container of reduce threads it's output container 
typedef std::vector<REDUCE_THREAD> REDUCE_THREAD_VEC;

// input to the f_map function
typedef struct MAP_INPUT
{
    IN_ITEMS_VEC *input_vec;
    MapReduceBase *mapReduceBase_object;
    int thread_num;
}MAP_INPUT;

// ***************************** global vairables ***************************************
MAP_THREAD_VEC map_thread2Container;
REDUCE_THREAD_VEC reduce_thread2Container;
pthread_mutex_t pthread2Container_mutex;

pthread_t shuffle_th;
pthread_t *map_threads;
pthread_t *reduce_threads;

sem_t shuffle_read_sem;

pthread_mutex_t pos_map_mutex;
pthread_mutex_t pos_reduce_mutex;

SHUFFLE_VEC shuffle_prod_vec;
auto cmpLambdak2 = [](const k2Base const *lhs, const k2Base const *rhs) { return *lhs < *rhs; };
//You could also use a lambda that is not dependent on local variables, like this:
//auto cmpLambda = [](const Point &lhs, const Point &rhs) { return lhs.y < rhs.y; };
std::map<k2Base*, V2_VEC, decltype(cmpLambdak2)> shuffle_prod(cmpLambdak2);

//output thread log
std::ofstream slog;

// ***************************** function declarations **********************************

OUT_ITEMS_VEC merge_reduce_vec();
void shuffleMap2Vec(std::map<k2Base*, V2_VEC, decltype(cmpLambdak2)> &m, SHUFFLE_VEC &v);
void* f_map(void *arg);
void* f_shuffle(void *arg);
void* f_reduce(void *arg);
void new_thread_log(std::string);
bool k3_comperator(OUT_ITEM first, OUT_ITEM second);


// **************************** function implementations ********************************
/**
 * runs the map reduce procedure in multi thread parallel mode
 * @param mapReduce - the class that implemented the map and reduce functions
 * @param itemsVec - the input to the map function
 * @param multiThreadLevel - the number of map and reduce thread
 * @param autoDeleteV2K2 - a flag to delete the map function output
 * @return
 */
OUT_ITEMS_VEC RunMapReduceFramework(MapReduceBase& mapReduce, IN_ITEMS_VEC& itemsVec,
                                    int multiThreadLevel, bool autoDeleteV2K2)
{

    slog = std::ofstream(LOG_FILE_NAME, std::ios_base::app);
    slog << INTRO_MSG1 << multiThreadLevel << INTRO_MSG2 << std::endl;

    // initialize map & reduce threads; 
    map_threads = new pthread_t[multiThreadLevel];
    reduce_threads = new pthread_t[multiThreadLevel];

    // initialize mutex protectors;
    pos_map_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread2Container_mutex = PTHREAD_MUTEX_INITIALIZER;
    pos_reduce_mutex = PTHREAD_MUTEX_INITIALIZER;

    // initialize read semaphore
    sem_init(&shuffle_read_sem, 0, 0);

    // initialize the f_map function input
    MAP_INPUT map_input;
    map_input.input_vec = &itemsVec;
    map_input.mapReduceBase_object = &mapReduce;
    
    // lock the map and shuffle threads from start running before all are initialized
    pthread_mutex_lock(&pthread2Container_mutex);

    //creates shuffle threads
    if(pthread_create(&shuffle_th, NULL, f_shuffle, &autoDeleteV2K2))
    {
        //TODO err
    }
    new_thread_log(SHUFFLE_THREAD_NAME);

    // creates map threads
    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        map_input.thread_num = i;
        if(pthread_create(&map_threads[i], NULL, f_map, (void*) &map_input))
        {
            //TODO err
        }
        new_thread_log(MAP_THREAD_NAME);
        MAP_ITEM_VEC temp_vec;
        map_thread2Container.push_back(new ThreadToContainer(&map_threads[i], temp_vec, 0, false));
    }
    pthread_mutex_unlock(&pthread2Container_mutex);

    // finish mapping
    for (int i = 0; i <  multiThreadLevel; ++i)
    {
        if(pthread_join(map_threads[i], NULL))
        {
            //TODO err
        }
    }

    //increase semaphore to release shuffle thread
    sem_post(&shuffle_read_sem);
    shuffleMap2Vec(shuffle_prod ,shuffle_prod_vec);

    // deletes mutex
    if(pthread_mutex_destroy(&pos_map_mutex))
    {
        //TODO err
    }

    // finish shuffling
    pthread_join(shuffle_th, NULL);

    // creates reduce threads
    // 2 different loops to ensure initializing before threads start running
    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        OUT_ITEMS_VEC temp_vec;
        reduce_thread2Container.push_back(std::make_pair(&reduce_threads[i], temp_vec));
    }
    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        if(pthread_create(&reduce_threads[i], NULL, f_reduce, &mapReduce))
        {
            //TODO err
        }
        new_thread_log(REDUCE_THREAD_NAME);

    }

    //finish reducing
    for (int j = 0; j < multiThreadLevel ; ++j)
    {
        pthread_join(reduce_threads[j], NULL);
    }

    // destroy reduce mutex
    if(pthread_mutex_destroy(&pos_reduce_mutex))
    {
        //TODO err
    }

    //deletes map procedure output
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

    // deletes all locked pointers
    slog.flush();
    slog.close();
    for (int i = 0; i < map_thread2Container.size() ; ++i)
    {
        delete(map_thread2Container[i]);
    }
    delete[](map_threads);
    delete[](reduce_threads);

    return merge_reduce_vec();
}

/**
 * map thread function - runs the map procedure for each thread
 * @param arg - pointer to MAP_INPUT struct as void pointer
 * @return NULL
 */
void* f_map(void *arg)
{
    //lock & unlock to ensure no thread is map thread is running before all  map and
    // shuffle are initialized
    pthread_mutex_lock(&pthread2Container_mutex);
    pthread_mutex_unlock(&pthread2Container_mutex);

    // position on map input
    static unsigned int pos = 0;
    MAP_INPUT *map_input = (MAP_INPUT*)arg;

    while(true)
    {
        // protects the position place holder
        if(pthread_mutex_lock(&pos_map_mutex))
        {
            //TODO err
        }
        // finished mapping all input
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

        if(pthread_mutex_unlock(&pos_map_mutex))
        {
            //TODO err
        }

        unsigned int end_point = start_point + WORK_CHUNCK;

        if(end_point >= map_input->input_vec->size())
        {
            end_point = map_input->input_vec->size();
        }

        for (unsigned int i = start_point; i < end_point; ++i)
        {

            map_input->mapReduceBase_object->Map(map_input->input_vec->at(i).first, map_input->input_vec->at(i).second);
        }

    }

    // updates map to container
    for (unsigned int i = 0; i < map_thread2Container.size(); ++i)
    {
        if(pthread_equal(*map_thread2Container[i]->_ithread, pthread_self()))
        {
            pthread_mutex_lock(&map_thread2Container[i]->_cont_mutex);
            map_thread2Container[i]->_thread_complete = true;
            pthread_mutex_unlock(&map_thread2Container[i]->_cont_mutex);
            break;
        }
    }
    return NULL;
}

/**
 * shuffle thread function - runs the shuffle procedure for each thread
 * @param arg - auto delete flag. bool pointer as void pointer
 * @return NULL
 */
void* f_shuffle(void *arg)
{
    bool autoDelete = *(bool*)arg;

    // all map threads complete flag
    bool map_complete = true;
    while(map_complete)
    {
        // waits until mapped item is available
        sem_wait(&shuffle_read_sem);
        sem_post(&shuffle_read_sem);

        for (unsigned int i = 0; i < map_thread2Container.size() ; ++i)
        {
            //protects from reading and writing at the same time
            pthread_mutex_lock(&map_thread2Container[i]->_cont_mutex);
            //all map threads are complete
            map_complete = map_complete && map_thread2Container[i]->_thread_complete;
            int dif  = map_thread2Container[i]->_container.size() - map_thread2Container[i]->_read_pos;
            pthread_mutex_unlock(&map_thread2Container[i]->_cont_mutex);

            //available item in the thread's container
            if(dif > 0)
            {
                for (int j = map_thread2Container[i]->_read_pos;
                     j < map_thread2Container[i]->_read_pos + dif;
                     ++j)
                {

                    bool found = false;
                    pthread_mutex_lock(&map_thread2Container[i]->_cont_mutex);
                    k2Base* key = map_thread2Container[i]->_container[j].first;
                    v2Base* val = map_thread2Container[i]->_container[j].second;
                    pthread_mutex_unlock(&map_thread2Container[i]->_cont_mutex);
                    V2_VEC *val2_vec = &shuffle_prod[key];
                    val2_vec->push_back(val);

//                    for (unsigned int k = 0; k < shuffle_prod_vec.size(); ++k)
//                    {
//
//                        if (!(*key < *shuffle_prod_vec[k].first) &&
//                            !(*shuffle_prod_vec[k].first < *key))
//                        {
//                            pthread_mutex_lock(&map_thread2Container[i]->_cont_mutex);
//                            shuffle_prod_vec[k].second.push_back(map_thread2Container[i]->_container[j].second);
//                            if(autoDelete)
//                            {
//                                delete(key);
//                            }
//                            pthread_mutex_unlock(&map_thread2Container[i]->_cont_mutex);
//                            found = true;
//                            break;
//                        }
//                    }
//                    if(!found)
//                    {
//                        V2_VEC temp;
//                        v2Base *val = map_thread2Container[i]->_container[j].second;
//                        temp.push_back(val);
////                       temp.push_back(map_thread2Container[i]->_container[j].second);
//                        shuffle_prod_vec.push_back(std::make_pair(map_thread2Container[i]->_container[j].first, temp));
//                    }
                    sem_wait(&shuffle_read_sem);
                }
                map_thread2Container[i]->_read_pos += dif;
            }
        }
        map_complete = !map_complete;
    }
    return NULL;
}

/**
 * reduce thread function - runs the reduce procedure for each thread
 * @param arg - A pointer to mapReduceBase class as void pointer
 * @return NULL
 */
void* f_reduce(void *arg)
{
    // position on shuffle product vector output
    static unsigned int pos = 0;
    MapReduceBase *mapReduceBase = (MapReduceBase*)arg;

    while(true)
    {
        //protect the position
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
        if(pthread_mutex_unlock(&pos_reduce_mutex))
        {
            //TODO err
        }
        unsigned int end_point = start_point + WORK_CHUNCK;
        if(end_point >= shuffle_prod_vec.size())
        {
            end_point = shuffle_prod_vec.size();
        }

        for (unsigned int i = start_point; i < end_point; ++i)
        {

            mapReduceBase->Reduce(shuffle_prod_vec[i].first, shuffle_prod_vec[i].second);
        }
    }
    return NULL;
}

/**
 * Emits the map function output to the right contianter by the running thread
 * @param key - k2Base pointer
 * @param value -v2Base pointer
 */
void Emit2 (k2Base* key, v2Base* value)
{
    // increase the semaphore by 1
    sem_post(&shuffle_read_sem);

    for (unsigned int i = 0; i < map_thread2Container.size(); ++i)
    {
        pthread_mutex_lock(&map_thread2Container[i]->_cont_mutex);
        if(pthread_equal(*map_thread2Container[i]->_ithread, pthread_self()))
        {
           map_thread2Container[i]->_container.push_back(std::make_pair(key, value));
           pthread_mutex_unlock(&map_thread2Container[i]->_cont_mutex);
           break;
        }
        pthread_mutex_unlock(&map_thread2Container[i]->_cont_mutex);
    }
}

/**
 * Emits the reduce function output to the right contianter by the running thread
 * @param key - k3Base pointer
 * @param value - v3Base pointer
 */
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

/**
 *
 * @return
 */
OUT_ITEMS_VEC merge_reduce_vec()
{
    for (unsigned int i = 1; i < reduce_thread2Container.size(); ++i)
    {
        reduce_thread2Container[0].second.insert(reduce_thread2Container[0].second.end(),
                                                 reduce_thread2Container[i].second.begin(),
                                                 reduce_thread2Container[i].second.end());
    }
    std::sort(reduce_thread2Container[0].second.begin(), reduce_thread2Container[0].second.end(), k3_comperator);
    return reduce_thread2Container[0].second;
}

void shuffleMap2Vec(std::map<k2Base*, V2_VEC, decltype(cmpLambdak2)> &m, SHUFFLE_VEC &v)
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

bool k3_comperator(OUT_ITEM first, OUT_ITEM second)
{
    return *first.first < *second.first;
}