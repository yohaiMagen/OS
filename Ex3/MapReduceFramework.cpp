#include <pthread.h>
#include <algorithm>
#include <unordered_map>
#include <semaphore.h>
#include <map>
#include <fstream>
#include <ctime>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include "ThreadToContainer.h"
#include "MapReduceFramework.h"


#define WORK_CHUNCK 10

// *********************************** log messages *************************************
#define LOG_FILE_NAME ".MapReduceFramework.log"
#define INTRO_MSG1 "RunMapReduceFramework started with "
#define INTRO_MSG2 " threads"

#define SHUFFLE_THREAD_NAME "shuffle"
#define MAP_THREAD_NAME "ExacMap"
#define REDUCE_THREAD_NAME "ExacReduce"

#define NEW_THREAD "created"
#define DELETE_THREAD "terminated"
#define MAP_TIME "Map and Shuffle took "
#define REDUCE_TIME "Reduce took "
#define END_STATEMENT "RunMapReduceFramework finished"
#define ERR_MSG "MapReduceFramework Failure: "
#define ERR_MSG_END " failed."

// *********************************** err functions names ******************************
#define NEW "new"

#define GETTIMEOFDAY "gettimeofday"

#define SEM_INIT "sem_init"
#define SEM_WAIT "sem_wait"
#define SEM_POST "sem_post"

#define PTH_LOCK "pthread_mutex_lock"
#define PTH_UNLOCK "pthrea_mutex_unlock"
#define PTH_CREATE "pthread_create"
#define PTH_JOIN "pthread_join"
#define PTH_DESTROY "pthread_destroy"
// *********************************** typedefs *****************************************
// container of thread and its output container (for shuffle thread) 
typedef std::pair<pthread_t*, OUT_ITEMS_VEC> REDUCE_THREAD;

// container of map threads, it's output container and more features
typedef std::vector<ThreadToContainer*> MAP_THREAD_VEC;

// container of reduce threads it's output container 
typedef std::vector<REDUCE_THREAD> REDUCE_THREAD_VEC;

// input to the f_map function
typedef struct F_MAP_INPUT
{
    IN_ITEMS_VEC *input_vec;
    MapReduceBase *mapReduceBase_object;
}F_MAP_INPUT;

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
pthread_mutex_t log_mutex;

auto cmpLambdak2 = [](const k2Base* const lhs, const k2Base* const rhs) { return *lhs < *rhs;  };
//You could also use a lambda that is not dependent on local variables, like this:
//auto cmpLambda = [](const Point &lhs, const Point &rhs) { return lhs.y < rhs.y; };
std::map<k2Base*, V2_VEC, decltype(cmpLambdak2)> shuffle_prod(cmpLambdak2);

//output thread log
std::ofstream slog;

// ***************************** function declarations **********************************

OUT_ITEMS_VEC merge_reduce_vec();
void* f_map(void *arg);
void* f_shuffle(void *arg);
void* f_reduce(void *arg);
bool k3_comperator(OUT_ITEM first, OUT_ITEM second);
void thread_log(std::string type, std::string CorD);
void delete_thread_log(std::string type);
void new_thread_log(std::string);
long thread_time(timeval start, timeval end);

// **************************** function implementations ********************************


void err_msg(std::string gettimeofday);

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

    timeval map_start;
    if(gettimeofday(&map_start, NULL))
    {
        err_msg(GETTIMEOFDAY);
    }

    slog = std::ofstream(LOG_FILE_NAME, std::ios_base::app);
    slog << INTRO_MSG1 << multiThreadLevel << INTRO_MSG2 << std::endl;

    // initialize map & reduce threads;
    try
    {
        map_threads = new pthread_t[multiThreadLevel];
        reduce_threads = new pthread_t[multiThreadLevel];
    }
    catch(std::bad_alloc)
    {
        err_msg(NEW);
    }

    // initialize mutex protectors;
    log_mutex = PTHREAD_MUTEX_INITIALIZER;
    pos_map_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread2Container_mutex = PTHREAD_MUTEX_INITIALIZER;
    pos_reduce_mutex = PTHREAD_MUTEX_INITIALIZER;

    // initialize read semaphore
    if(sem_init(&shuffle_read_sem, 0, 0))
    {
        err_msg(SEM_INIT);
    }

    // initialize the f_map function input
    F_MAP_INPUT f_map_input;
    f_map_input.input_vec = &itemsVec;
    f_map_input.mapReduceBase_object = &mapReduce;
    
    // lock the map and shuffle threads from start running before all are initialized
    if(pthread_mutex_lock(&pthread2Container_mutex))
    {
        err_msg(PTH_LOCK);
    }

    //creates shuffle threads
    if(pthread_create(&shuffle_th, NULL, f_shuffle, &autoDeleteV2K2))
    {
        err_msg(PTH_CREATE);
    }

    // creates map threads
    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        if(pthread_create(&map_threads[i], NULL, f_map, (void*) &f_map_input))
        {
            err_msg(PTH_CREATE);
        }
        MAP_ITEM_VEC temp_vec;
        try
        {
            map_thread2Container.push_back(new ThreadToContainer(&map_threads[i], temp_vec, 0, false));
        }
        catch(std::bad_alloc)
        {
            err_msg(NEW);
        }

    }
    if(pthread_mutex_unlock(&pthread2Container_mutex))
    {
        err_msg(PTH_UNLOCK);
    }

    // finish mapping
    for (int i = 0; i <  multiThreadLevel; ++i)
    {
        if(pthread_join(map_threads[i], NULL))
        {
            err_msg(PTH_JOIN);
        }
    }

    //increase semaphore to release shuffle thread
    if(sem_post(&shuffle_read_sem))
    {
        err_msg(SEM_POST);
    }

    // deletes mutex
    if(pthread_mutex_destroy(&pos_map_mutex))
    {
        err_msg(PTH_DESTROY);
    }

    // finish shuffling
    if(pthread_join(shuffle_th, NULL))
    {
        err_msg(PTH_JOIN);
    }

    // creates reduce threads
    // 2 different loops to ensure initializing before threads start running
    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        OUT_ITEMS_VEC temp_vec;
        reduce_thread2Container.push_back(std::make_pair(&reduce_threads[i], temp_vec));
    }
    timeval map_end;
    if(gettimeofday(&map_end, NULL))
    {
        err_msg(GETTIMEOFDAY);
    }
    slog << MAP_TIME << thread_time(map_start, map_end) << " ns" <<  std::endl;

    for (int i = 0; i < multiThreadLevel ; ++i)
    {
        if(pthread_create(&reduce_threads[i], NULL, f_reduce, &mapReduce))
        {
            err_msg(PTH_CREATE);
        }
    }

    //finish reducing
    for (int j = 0; j < multiThreadLevel ; ++j)
    {
        if(pthread_join(reduce_threads[j], NULL))
        {
            err_msg(PTH_JOIN);
        }
    }

    // destroy reduce mutex
    if(pthread_mutex_destroy(&pos_reduce_mutex))
    {
        err_msg(PTH_DESTROY);
    }

    //deletes map procedure output
    if(autoDeleteV2K2)
    {
        for (auto it = shuffle_prod.begin(); it != shuffle_prod.end(); ++it)
        {
            for (unsigned int j = 0; j < it->second.size(); ++j)
            {
                delete(it->second[j]);
            }
           delete(it->first);

        }
    }

    // deletes all locked pointers
    for (unsigned int i = 0; i < map_thread2Container.size() ; ++i)
    {
        delete(map_thread2Container[i]);
    }
    delete[](map_threads);
    delete[](reduce_threads);

    timeval end_reduce;
    gettimeofday(&end_reduce, NULL);
    slog << REDUCE_TIME << thread_time(map_end, end_reduce) << " ns" << std::endl;

    slog << END_STATEMENT << std::endl;

    slog.flush();
    slog.close();

    return merge_reduce_vec();
}


/**
 * map thread function - runs the map procedure for each thread
 * @param arg - pointer to MAP_INPUT struct as void pointer
 * @return NULL
 */
void* f_map(void *arg)
{
    if(pthread_mutex_lock(&log_mutex))
    {
        err_msg(PTH_LOCK);
    }
    new_thread_log(MAP_THREAD_NAME);
    if(pthread_mutex_unlock(&log_mutex))
    {
        err_msg(PTH_UNLOCK);
    }
    //lock & unlock to ensure no thread is map thread is running before all  map and
    // shuffle are initialized
    if(pthread_mutex_lock(&pthread2Container_mutex))
    {
        err_msg(PTH_LOCK);
    }
    if(pthread_mutex_unlock(&pthread2Container_mutex))
    {
        err_msg(PTH_UNLOCK);
    }

    // position on map input
    static unsigned long pos = 0;
    F_MAP_INPUT *map_input = (F_MAP_INPUT*)arg;

    while(true)
    {
        // protects the position place holder
        if(pthread_mutex_lock(&pos_map_mutex))
        {
            err_msg(PTH_LOCK);
        }
        // finished mapping all input
        if(pos >= map_input->input_vec->size())
        {
            if(pthread_mutex_unlock(&pos_map_mutex))
            {
                err_msg(PTH_UNLOCK);
            }
            break;
        }

        unsigned long start_point = pos;
        pos += WORK_CHUNCK;

        if(pthread_mutex_unlock(&pos_map_mutex))
        {
            err_msg(PTH_UNLOCK);
        }

        unsigned long end_point = start_point + WORK_CHUNCK;

        if(end_point >= map_input->input_vec->size())
        {
            end_point = map_input->input_vec->size();
        }

        for (unsigned long i = start_point; i < end_point; ++i)
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

    if(pthread_mutex_lock(&log_mutex))
    {
        err_msg(PTH_LOCK);
    }
    delete_thread_log(MAP_THREAD_NAME);
    if(pthread_mutex_unlock(&log_mutex))
    {
        err_msg(PTH_UNLOCK);
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
    if(pthread_mutex_lock(&log_mutex))
    {
        err_msg(PTH_LOCK);
    }
    new_thread_log(SHUFFLE_THREAD_NAME);
    if(pthread_mutex_unlock(&log_mutex))
    {
        err_msg(PTH_UNLOCK);
    }
    bool autoDelete = *(bool*)arg;

    // all map threads complete flag
    bool map_complete = true;
    while(map_complete)
    {
        // waits until mapped item is available
        if(sem_wait(&shuffle_read_sem))
        {
            err_msg(SEM_WAIT);
        }
        if(sem_post(&shuffle_read_sem))
        {
            err_msg(SEM_POST);
        }

        for (unsigned int i = 0; i < map_thread2Container.size() ; ++i)
        {
            //protects from reading and writing at the same time
            if(pthread_mutex_lock(&map_thread2Container[i]->_cont_mutex))
            {
                err_msg(PTH_LOCK);
            }
            //all map threads are complete
            map_complete = map_complete && map_thread2Container[i]->_thread_complete;
            long dif  = map_thread2Container[i]->_container.size() - map_thread2Container[i]->_read_pos;
            if(pthread_mutex_unlock(&map_thread2Container[i]->_cont_mutex))
            {
                err_msg(PTH_UNLOCK);
            }

            //available item in the thread's container
            if(dif > 0)
            {
                for (long j = map_thread2Container[i]->_read_pos;
                     j < map_thread2Container[i]->_read_pos + dif;
                     ++j)
                {

                    if(pthread_mutex_lock(&map_thread2Container[i]->_cont_mutex))
                    {
                        err_msg(PTH_LOCK);
                    }
                    k2Base* key = map_thread2Container[i]->_container[j].first;
                    v2Base* val = map_thread2Container[i]->_container[j].second;
                    if(pthread_mutex_unlock(&map_thread2Container[i]->_cont_mutex))
                    {
                        err_msg(PTH_UNLOCK);
                    }

                    V2_VEC *val2_vec = &shuffle_prod[key];

                    if(autoDelete && val2_vec->size() > 0)
                    {
                        delete(key);
                    }
                    val2_vec->push_back(val);

                    if(sem_wait(&shuffle_read_sem))
                    {
                        err_msg(SEM_WAIT);
                    }
                }
                map_thread2Container[i]->_read_pos += dif;
            }
        }
        map_complete = !map_complete;
    }
    if(pthread_mutex_lock(&log_mutex))
    {
        err_msg(PTH_LOCK);
    }
    delete_thread_log(SHUFFLE_THREAD_NAME);
    if(pthread_mutex_unlock(&log_mutex))
    {
        err_msg(PTH_UNLOCK);
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
    if(pthread_mutex_lock(&log_mutex))
    {
        err_msg(PTH_LOCK);
    }
    new_thread_log(REDUCE_THREAD_NAME);
    if(pthread_mutex_unlock(&log_mutex))
    {
        err_msg((PTH_UNLOCK));
    }

    // position on shuffle product
    static auto pos1 = shuffle_prod.begin();
    MapReduceBase *mapReduceBase = (MapReduceBase*)arg;

    while(true)
    {
        //protect the position
        if(pthread_mutex_lock(&pos_reduce_mutex))
        {
            err_msg(PTH_LOCK);
        }
        if(pos1 == shuffle_prod.end())
        {
            if(pthread_mutex_unlock(&pos_reduce_mutex))
            {
                err_msg(PTH_UNLOCK);
            }
            break;
        }

        auto  start_point1 = pos1;
        for (int i = 0; i < WORK_CHUNCK && pos1 != shuffle_prod.end() ; ++i)
        {
            pos1++;
        }
        auto end_point1 = pos1;

        if(pthread_mutex_unlock(&pos_reduce_mutex))
        {
            err_msg(PTH_LOCK);
        }

        for (; start_point1 != end_point1; ++start_point1)
        {
            mapReduceBase->Reduce(start_point1->first, start_point1->second);
        }

    }
    if(pthread_mutex_lock(&log_mutex))
    {
        err_msg(PTH_LOCK);
    }
    delete_thread_log(REDUCE_THREAD_NAME);
    if(pthread_mutex_unlock(&log_mutex))
    {
        err_msg(PTH_UNLOCK);
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
    if(sem_post(&shuffle_read_sem))
    {
        err_msg(SEM_POST);
    }

    for (unsigned int i = 0; i < map_thread2Container.size(); ++i)
    {
        if(pthread_mutex_lock(&map_thread2Container[i]->_cont_mutex))
        {
            err_msg(PTH_LOCK);
        }
        if(pthread_equal(*map_thread2Container[i]->_ithread, pthread_self()))
        {
           map_thread2Container[i]->_container.push_back(std::make_pair(key, value));
           if(pthread_mutex_unlock(&map_thread2Container[i]->_cont_mutex))
           {
               err_msg(PTH_UNLOCK);
           }
           break;
        }
        if(pthread_mutex_unlock(&map_thread2Container[i]->_cont_mutex))
        {
            err_msg(PTH_UNLOCK);
        }
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

void new_thread_log(std::string type)
{
    thread_log(type, NEW_THREAD);
}

void delete_thread_log(std::string type)
{
    thread_log(type, DELETE_THREAD);
}

void thread_log(std::string type, std::string CorD)
{
    time_t now =  time(0);
    tm *ltm = localtime(&now);
    slog << "Thread " << type << " " << CorD << " [" <<
        ltm->tm_mday << "." << ltm->tm_mon + 1 << "." << ltm->tm_year + 1900 << " " <<
        ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "]" << std::endl;

}

bool k3_comperator(OUT_ITEM first, OUT_ITEM second)
{
    return *first.first < *second.first;
}

void err_msg(std::string func_name)
{
    std::cerr << ERR_MSG << func_name << ERR_MSG_END << std::endl;
    exit(1);
}

long thread_time(timeval start, timeval end)
{
    return  ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) * 1000;
}
