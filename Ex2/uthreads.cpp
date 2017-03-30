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


//-------------------------------defines----------------------------------------
#define Q_INIT 0

//-------------------------------variables--------------------------------------

// thread handling data structure
std::map<unsigned int, thread> threads;
// round robin thread queue
std::queue<unsigned int> ready_queue;
// current quanta
unsigned int quanta;
//min heap next next thread
std::priority_queue<unsigned int> next_thread;
// current running thread
unsigned int curr_thread;

struct sigaction sa;
struct itimerval timer;


//-----------------------------------functions----------------------------------

unsigned int get_next_thread_id()
{
    if(next_thread.empty())
    {
        return threads.size() + 1;
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
    if (ready_queue.empty())
    {
        return;
    }

    int ret_val = sigsetjmp(threads[ready_queue.front()]._env,1);
    if (ret_val == 1) {
        return;
    }
    quanta++;
    threads[curr_thread]._state = ready;
    ready_queue.push(curr_thread);
    curr_thread = ready_queue.front();
    ready_queue.pop();
    threads[curr_thread]._num_quantum++;
    siglongjmp(threads[curr_thread]._env,1);
}

void erase_from_queue(unsigned int to_erase)
{
    std::queue<unsigned int> temp;
    while(ready_queue.empty())
    {
        if(ready_queue.front() == to_erase)
        {
            ready_queue.pop();
            break;
        }
        temp.push(ready_queue.front());
        ready_queue.pop();
    }
    while(!temp.empty())
    {
        ready_queue.push(temp.front());
        temp.pop();
    }
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
    quanta = Q_INIT;
    if (quantum_usecs <= 0)
    {
        return -1;
    }


    sa.sa_handler = &switch_threads;
    if (sigaction(SIGVTALRM, &sa, NULL) < 0)
    {
        printf("sigaction error.");
    }

    // Configure the timer to expire
    int q_time_usec = quantum_usecs / 1000000;
    int q_time_sec = quantum_usecs % 1000000;
    timer.it_value.tv_usec = q_time_usec;
    timer.it_value.tv_sec = q_time_sec;
    timer.it_interval.tv_usec = q_time_usec;
    timer.it_interval.tv_sec = q_time_sec;

    unsigned int id = get_next_thread_id();
    threads[id] = thread(id, 0, running);

    quanta++;
    threads[id]._num_quantum++;
    curr_thread = id;

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
    {
        fprintf(stderr, "setitimer error.", 16);
        return -1;

        return 0;
    }
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
        unsigned int id = get_next_thread_id();
        if (id > MAX_THREAD_NUM)
        {
            fprintf(stderr, "max thread num reached", 22);
            return -1;
        }

        threads.insert(std::pair(id, thread(id, (address_t) f)));
        ready_queue.push(id);

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
    if(tid == 0)
    {
        exit(0);
    }
    if(threads.find(tid) == std::map::end()|| tid < 0 )
    {
        return -1;
    }
    threads.erase(tid);
    erase_from_queue(tid);
    next_thread.push(tid);
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
    if(threads.find(tid) == std::map::end() || tid <= 0 )
    {
        return -1;
    }
    threads[tid]._state = waiting;
    erase_from_queue(tid);
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
    threads[tid]._state = ready;
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
    threads[tid]._num_quantum;
}

int main()
{
    return 0;
}