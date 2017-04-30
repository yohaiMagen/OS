#include <iostream>
#include "Thread.h"

/**
 * Default constructor
 */
Thread::Thread() : Thread(-1 , DUMMY_ADRESS, blockedNwaiting) {}

/**
 * Constructor
 * @param id unsigned int - thread's ID
 * @param pc address_t - starting point of the thread
 * @param state threadState - the state of the thread (default ready)
 * @param quantum unsigned int - the number of quantum to intial the thread with***
 */
Thread::Thread(unsigned int id, address_t pc, threadState state, unsigned int quantum)
{
    _id = id;
    _state = state;
    _num_quantum = quantum;

    address_t sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
    sigsetjmp(_env, 1);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&_env->__saved_mask);
}

/**
 * Empty Destructor
 */
Thread::~Thread() {}
