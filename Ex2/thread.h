//
// Created by yohai on 3/25/17.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H

#include "uthreads.h"
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <vector>


#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
inline address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
            "rol    $0x11,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}

#else

/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
inline address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
        "rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}
#endif


// thread state
enum threadState { ready, running, waiting, blocked, blockedNwaiting, selfTerminated };

/**
 * A simple thread
 */
struct thread
{
    std::vector<unsigned int> _waiting_for_me;
    unsigned int _id; // unique number
    char _stack[STACK_SIZE];
    sigjmp_buf _env; //thread's buffer
    char* _sp; // stack pointer
    char* _pc; // program counter
    threadState _state; // thread's state
    unsigned int _num_quantum; // number of quantum run by the thread

    thread();
    thread(unsigned int id, address_t pc, threadState state = ready, unsigned int quantum = 0);
    thread(const thread& other);
    thread operator=(const thread &other);
    ~thread();

}typedef thread;


#endif //EX2_THREAD_H