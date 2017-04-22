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
#define FAIL 1
#define LONG_JUMP_RETURN 4
#define MIL 1000000

#define SYS_ERR "system error: "
#define SIG_MASK_ERR "failed masking vt signal"
#define SIG_UNMASK_ERR "failed unmasking vt signal"
#define SET_TIMER_ERR "failed setting the virtual timer"
#define UTHREAD_ERR "thread library error: "
#define ILLEGAL_TIME "illegal quanta time, must be positive"
#define SIG_SET_ERR "could not set a function to vt signal"
#define MAX_THREAD_ERR "reached maximum number of threads"
#define ILL_THREAD_NUM "illegal thread number"
#define SELF_SYNC_ERR " a thread cannot switch itself to waiting state"
#define MAIN_SYNC_ERR "main thread cannot be switched to waiting state"
#define MAIN_BLOCK_ERR "main thread cannot be blocked"
//-------------------------------variables-------------------------------------
// thread handling data structure
std::map<unsigned int, thread> threads;
// round robin thread queue
std::queue<unsigned int> ready_queue;
int erase_from_queue[100];
// current quanta
unsigned int quanta;
//min heap next next thread
std::priority_queue<unsigned int, std::vector<unsigned int>, std::greater<unsigned int> > next_available_thread;
// current running thread
unsigned int curr_thread;

struct sigaction sa;
struct itimerval timer;
sigset_t blc_set;


//-----------------------------------functions----------------------------------

void block_signal()
{
    if(sigprocmask(SIG_BLOCK, &blc_set, NULL))
    {
        std::cerr << SYS_ERR << SIG_MASK_ERR << std::endl;
        exit(FAIL);
    }
}

void unblock_signal()
{
    if(sigprocmask(SIG_UNBLOCK, &blc_set, NULL))
    {
        std::cerr << SYS_ERR << SIG_UNMASK_ERR << std::endl;
        exit(FAIL);
    }
}

void reset_timer()
{
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
    {
        std::cerr << SYS_ERR << SET_TIMER_ERR << std::endl;
        exit(FAIL);
    }
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
    if(next_available_thread.empty())
    {
        return (unsigned int) threads.size();
    }
    else
    {
        unsigned int p = next_available_thread.top();
        next_available_thread.pop();
        return p;
    }
}

void switch_threads(int input)
{

    block_signal();
    if (ready_queue.empty())
    {
        threads[curr_thread]._num_quantum++;
        quanta++;
        unblock_signal();
        return;
    }
    int ret_val = sigsetjmp(threads[curr_thread]._env, 1);
    if (ret_val == LONG_JUMP_RETURN)
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
    uthread_unsync(curr_thread);
    if(threads[curr_thread]._state == selfTerminated)
    {
        threads.erase(curr_thread);
        next_available_thread.push(curr_thread);
    }
    else if(threads[curr_thread]._state == running)
    {
        ready_queue.push(curr_thread);
        threads[curr_thread]._state = ready;
    }
    curr_thread = next_thread;
    ready_queue.pop();
    threads[curr_thread]._num_quantum++;
    threads[curr_thread]._state = running;
    siglongjmp(threads[curr_thread]._env, LONG_JUMP_RETURN);
}

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
        std::cerr << UTHREAD_ERR << ILLEGAL_TIME << std::endl;
        return -1;
    }

    quanta = Q_INIT;
    sa.sa_handler = &switch_threads;
    sigemptyset(&blc_set);
    sigaddset(&blc_set, SIGVTALRM);
    if (sigaction(SIGVTALRM, &sa, NULL) < 0)
    {
        std::cerr << SYS_ERR << SIG_SET_ERR << std::endl;
        exit(-1);
    }

    // Configure the timer to expire
    int q_time_sec = quantum_usecs / MIL;
    int q_time_usec = quantum_usecs % MIL;
    timer.it_value.tv_usec = q_time_usec;
    timer.it_value.tv_sec = q_time_sec;
    timer.it_interval.tv_usec = q_time_usec;
    timer.it_interval.tv_sec = q_time_sec;

    unsigned int id = get_next_thread_id();
//    threads[id] = thread(id, 0, running);
    threads.insert(std::pair< int , thread>((int const)id, thread(id, 0, running)) );

    quanta++;
    threads[id]._num_quantum++;
    curr_thread = id;
    reset_timer();
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
        std::cerr << UTHREAD_ERR << MAX_THREAD_ERR << std::endl;
        unblock_signal();
        return -1;
    }
//    threads[id] = thread(id, (address_t) f);
    threads.insert(std::pair<int, thread>(id, thread(id, (address_t) f)));
    ready_queue.push(id);
    unblock_signal();
    return id;

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
        std::cerr << UTHREAD_ERR << ILL_THREAD_NUM << std::endl;
        unblock_signal();
        return -1;
    }

    if(threads[tid]._state == running)
    {
        threads[tid]._state = selfTerminated;
        reset_timer();
        switch_threads(0);
    }
    else
    {
        uthread_unsync(tid);
        if(threads[tid]._state == ready)
        {
            erase_from_queue[tid]++;
        }
        next_available_thread.push(tid);
        threads.erase(tid);
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
    if(threads.find(tid) == threads.end() || tid < 0 )
    {
        std::cerr << UTHREAD_ERR << ILL_THREAD_NUM << std::endl;
        return -1;
    }
    if(tid == 0)
    {
        std::cerr << UTHREAD_ERR << MAIN_BLOCK_ERR << std::endl;
        unblock_signal();
        return -1;
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
    if((unsigned int) tid == curr_thread)
    {
        reset_timer();
        switch_threads(0);
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
    if(threads.find(tid) == threads.end() || tid < 0 )
    {
        std::cerr << UTHREAD_ERR << ILL_THREAD_NUM << std::endl;
        unblock_signal();
        return -1;
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
    if(threads.find(tid) == threads.end() || tid < 0)
    {
        std::cerr << UTHREAD_ERR << ILL_THREAD_NUM << std::endl;
        unblock_signal();
        return -1;
    }
    if(curr_thread == 0)
    {
        std::cerr << UTHREAD_ERR << MAIN_SYNC_ERR << std::endl;
        unblock_signal();
        return -1;
    }
    if((unsigned int) tid == curr_thread)
    {
        std::cerr << UTHREAD_ERR << SELF_SYNC_ERR << std::endl;
        unblock_signal();
        return -1;
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
    reset_timer();
    switch_threads(0);
    return 0;
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
    if(tid < 0 || threads.find(tid) == threads.end())
    {
        std::cerr << UTHREAD_ERR << ILL_THREAD_NUM << std::endl;
        unblock_signal();
        return -1;
    }
    int quantum = threads[tid]._num_quantum;
    unblock_signal();
    return quantum;
}