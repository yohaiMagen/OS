#include "uthreads.h"
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <vector>
#include <map>
#include <queue>
#include "thread.h"
#include <cstdlib>
#include <iostream>


//-------------------------------defines----------------------------------------
#define Q_INIT 0

//-------------------------------variables-------------------------------------
// thread handling data structure
std::map<unsigned int, thread> threads;
// round robin thread queue
std::queue<unsigned int> ready_queue;
int erase_from_queue[100];
// current quanta
unsigned int quanta;
//min heap next next thread
std::priority_queue<unsigned int> next_thread;
// current running thread
unsigned int curr_thread;

struct sigaction sa;
struct itimerval timer;
sigset_t blc_set;


//-----------------------------------functions----------------------------------

void block_signal()
{
    if(sigprocmask(SIG_SETMASK, &blc_set, NULL))
    {
        //TODO
        exit(-1);
    }
}

void unblock_signal()
{
    if(sigprocmask(SIG_UNBLOCK, &blc_set, NULL))
    {
        //TODO
        exit(-1);
    }
}

int reset_timer()
{
    timer.it_value.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
    {
        fprintf(stderr, "setitimer error.");
        return -1;
    }
    return 0;
}


void uthread_unsync(unsigned int tid)
{
    for (std::vector<unsigned int>::iterator i = threads[tid]._waiting_for_me.begin(); i != threads[tid]._waiting_for_me.end(); ++i)
    {
        if(threads[*i]._state == blockedNwaiting)
        {
            threads[*i]._state = blocked;
        }
        else
        {
            threads[*i]._state = ready;
            ready_queue.push(*i);
        }
    }
    threads[tid]._waiting_for_me.clear();
}

unsigned int get_next_thread_id()
{
    if(next_thread.empty())
    {
        return (unsigned int) threads.size();
    }
    else
    {
        unsigned int p = next_thread.top();
        next_thread.pop();
        return p;
    }
}

void switch_threads(int input)
{

    block_signal();
//    std::cout << "switch" << std::endl;
    if (ready_queue.empty())
    {
        threads[curr_thread]._num_quantum++;
        quanta++;
        unblock_signal();
        return;
    }
    int ret_val = sigsetjmp(threads[curr_thread]._env, 1);
    if (ret_val == 4)
    {
        unblock_signal();
        return;
    }
    unsigned int next_thread = ready_queue.front();
    while(erase_from_queue[next_thread] != 0)
    {
        ready_queue.pop();
        erase_from_queue[next_thread]--;
        if(ready_queue.empty())
        {
            unblock_signal();
            return;
        }
        next_thread = ready_queue.front();
    }

    quanta++;
    threads[curr_thread]._state = ready;
    ready_queue.push(curr_thread);
    uthread_unsync(curr_thread);
    curr_thread = next_thread;
    ready_queue.pop();
    threads[curr_thread]._num_quantum++;
    threads[curr_thread]._state = running;
    siglongjmp(threads[curr_thread]._env, 4);
}


//void erase_from_queue_func(unsigned int to_erase)
//{
//    std::queue<unsigned int> temp;
//    while(ready_queue.empty())
//    {
//        if(ready_queue.front() == to_erase)
//        {
//            ready_queue.pop();
//            break;
//        }
//        temp.push(ready_queue.front());
//        ready_queue.pop();
//    }
//    while(!temp.empty())
//    {
//        ready_queue.push(temp.front());
//        temp.pop();
//    }
//}


/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantum in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs)
{
    if (quantum_usecs <= 0)
    {
        return -1;//TODO;
    }

    quanta = Q_INIT;
    sa.sa_handler = &switch_threads;
    sigemptyset(&blc_set);
    sigaddset(&blc_set, SIGVTALRM);
//    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGVTALRM, &sa, NULL) < 0)
    {
        printf("sigaction error.");
        exit(-1);
    }

    // Configure the timer to expire
    int q_time_sec = quantum_usecs / 1000000;
    int q_time_usec = quantum_usecs % 1000000;
    timer.it_value.tv_usec = 0;//TODO add time to exit init
    timer.it_value.tv_sec = 1;
    timer.it_interval.tv_usec = q_time_usec;
    timer.it_interval.tv_sec = q_time_sec;

    unsigned int id = get_next_thread_id();
//    threads[id] = thread(id, 0, running);
    threads.insert(std::pair< int , thread>((int const)id, thread(id, 0, running)) );

    quanta++;
    threads[id]._num_quantum++;
    curr_thread = id;

    if (setitimer (ITIMER_VIRTUAL, &timer, NULL))
    {
        printf("setitimer error.");
        exit(-1);
    }
    return 0;
}

/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (*f)(void))
{
    block_signal();
    unsigned int id = get_next_thread_id();
    if (id > MAX_THREAD_NUM)
    {
        fprintf(stderr, "max thread num reached");
        unblock_signal();
        return -1;//TODO;
    }
//    threads[id] = thread(id, (address_t) f);
    threads.insert(std::pair<int, thread>(id, thread(id, (address_t) f)));
    ready_queue.push(id);
    unblock_signal();
    return 0;

}


/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released. If no thread with ID tid
 * exists it is considered as an error. Terminating the main thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
*/
int uthread_terminate(int tid)
{
    block_signal();
    if(tid == 0)
    {
        unblock_signal();
        exit(0);
    }
    if(threads.find(tid) == threads.end() || tid < 0 )
    {
        unblock_signal();
        return -1;//TODO;
    }
    uthread_unsync(tid);
    if(threads[tid]._state == ready)
    {
        erase_from_queue[tid]++;
    }
    next_thread.push(tid);
    threads.erase(tid);
    if(tid == curr_thread)
    {
        unblock_signal();
        return reset_timer();
    }
    unblock_signal();
    return 0;
}


/*
 * Description: This function blocks the thread with ID tid. The thread may
 * be resumed later using uthread_resume. If no thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision
 * should be made. Blocking a thread in BLOCKED state has no
 * effect and is not considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_block(int tid)
{
    block_signal();
    if(threads.find(tid) == threads.end() || tid <= 0 )
    {
        return -1;//TODO;
    }
    if(threads[tid]._state == waiting)
    {
        threads[tid]._state = blockedNwaiting;
    }
    else
    {
        if(threads[tid]._state == ready)
        {
            erase_from_queue[tid]++;
        }
        threads[tid]._state = blocked;
    }
    if(tid == curr_thread)
    {
        unblock_signal();
        return reset_timer();
    }
    unblock_signal();
    return 0;
}


/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state. Resuming a thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid)
{
    block_signal();
    if(threads.find(tid) == threads.end() || tid <= 0 || tid == curr_thread)
    {
        return -1;//TODO;
    }
    if(threads[tid]._state == blockedNwaiting)
    {
        threads[tid]._state = waiting;
    }
    else if(threads[tid]._state == blocked)
    {
        threads[tid]._state = ready;
        ready_queue.push(tid);
    }
    unblock_signal();
    return 0;
}


/*
 * Description: This function blocks the RUNNING thread until thread with
 * ID tid will move to RUNNING state (i.e.right after the next time that
 * thread tid will stop running, the calling thread will be resumed
 * automatically). If thread with ID tid will be terminated before RUNNING
 * again, the calling thread should move to READY state right after thread
 * tid is terminated (i.e. it won’t be blocked forever). It is considered
 * as an error if no thread with ID tid exists or if the main thread (tid==0)
 * calls this function. Immediately after the RUNNING thread transitions to
 * the BLOCKED state a scheduling decision should be made.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_sync(int tid)
{
    block_signal();
    if(tid == curr_thread)
    {
        unblock_signal();
        return -1;//TODO;
    }
    threads[tid]._waiting_for_me.push_back(curr_thread);
    if (threads[curr_thread]._state == blocked)
    {
        threads[curr_thread]._state = blockedNwaiting;
    }
    else
    {
        threads[curr_thread]._state = waiting;
    }
    unblock_signal();
    return reset_timer();
}

/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid() {
    return curr_thread;
}

/*
 * Description: This function returns the total number of quantums that were
 * started since the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums()
{
    return quanta;
}

/*
 * Description: This function returns the number of quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * thread with ID tid exists it is considered as an error.
 * Return value: On success, return the number of quantums of the thread with ID tid. On failure, return -1.
*/
int uthread_get_quantums(int tid)
{
    block_signal();
    if(threads.find(tid) == threads.end())
    {
        unblock_signal();
        return -1;//TODO
    }
//    std::cout << "whay" << std::endl;
    unblock_signal();
    return threads[tid]._num_quantum;
}
//
//void f(void)
//{
//    int i = 0;
//    while(1){
//        ++i;
//        for (int j = 0; j < 99999; ++j) { }
//        std::cout << "in f " << i << std::endl;
////        usleep(10000);
//    }
//}
//
//void g(void)
//{
//    int i = 0;
//    while(1){
//        ++i;
//        for (int j = 0; j < 99999; ++j) { }
//        std::cout << "in g " << i << std::endl;
//        std::cout.flush();
////        usleep(10000);
//    }
//}
//
//
//
//int main(void)
//{
//
//    uthread_init(1000);
//    uthread_spawn(f);
//    uthread_spawn(g);
//    int i = 0;
//    while(1)
//    {
//        ++i;
//        for (int j = 0; j < 999999; ++j) { }
//        std::cout << "in main " << i << std::endl;
////        kill(0, SIGVTALRM);
////        usleep(10000);
////        uthread_sync(1);
////        uthread_sync(2);
//    }
//
//}




//void switchThreads(int a)
//{
//    static int currentThread = 0;
//
//    int ret_val = sigsetjmp(test[currentThread]._env,1);
//    printf("SWITCH: ret_val=%d\n", ret_val);
//    if (ret_val == 1) {
//        return;
//    }
//    currentThread = 1 - currentThread;
//    siglongjmp(test[currentThread]._env,1);
//}
//
//void f(void)
//{
//    int i = 0;
//    while(1){
//        ++i;
//        printf("in f (%d)\n",i);
////        if (i % 3 == 0) {
////            printf("f: switching\n");
////            switchThreads();
////        }
////        usleep(300000);
//        for (int j = 0; j < 100000; ++j) {
//
//        }
//    }
//}
//
//void g(void)
//{
//    int i = 0;
//    while(1){
//        ++i;
//        printf("in g (%d)\n",i);
////        if (i % 5 == 0) {
////            printf("g: switching\n");
////            switchThreads();
////        }
////        usleep(300000);
//        for (int j = 0; j < 100000; ++j) {
//
//        }
//    }
//}
//
//void setup(void)
//{
//    address_t sp, pc;
//
//    test[0] = thread(0, (address_t)f);
//
//
//
//    test[1] = thread(1, (address_t)g);
//
//    sa.sa_handler = &switchThreads;
//    sigemptyset(&blc_set);
//    sigaddset(&blc_set, SIGVTALRM);
////    sa.sa_flags = SA_SIGINFO;
//    if (sigaction(SIGVTALRM, &sa, NULL) < 0)
//    {
//        printf("sigaction error.");
//        exit(-1);
//    }
//
//    // Configure the timer to expire
//    int q_time_sec = 0;
//    int q_time_usec = 1000;
//    timer.it_value.tv_usec = 100;//TODO add time to exit init
//    timer.it_value.tv_sec = 0;
//    timer.it_interval.tv_usec = q_time_usec;
//    timer.it_interval.tv_sec = q_time_sec;
//    if (setitimer (ITIMER_VIRTUAL, &timer, NULL))
//    {
//        printf("setitimer error.");
//        exit(-1);
//    }
//}
//
//int main(void)
//{
//    setup();
//    f();
//    return 0;
//}