#include <iostream>
#include "Thread.h"

/**
 * Default constructor
 */
Thread::Thread() : Thread(-1 , NULL, blockedNwaiting) {}

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
 * copy contructor - to delete
 */
//Thread::Thread(const Thread& other) //: thread(other._id, other._env->__jmpbuf[JB_PC], other._state, other._num_quantum)
//{
//    std::cout<<"copy: "<< other._id <<std::endl;
//    _id = other._id;
//    address_t sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
//    sigsetjmp(_env, 1);
//    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
//    (_env->__jmpbuf)[JB_PC] = other._env->__jmpbuf[JB_PC];
//    sigemptyset(&_env->__saved_mask);
//    _state = other._state;
//    _num_quantum = other._num_quantum;
//}

Thread::~Thread() {}

/**
 * operator= - to delete
 * @param other
 * @return
 */
//Thread Thread::operator=(const Thread &other) {
//    std::cout<<"operator="<<std::endl;
//    address_t sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
//    sigsetjmp(_env, 1);
//    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
//    (_env->__jmpbuf)[JB_PC] = other._env->__jmpbuf[JB_PC];
//    sigemptyset(&_env->__saved_mask);
//    _state = other._state;
//    _num_quantum = other._num_quantum;
//    return *this;
//}
//thread::thread(thread &&other)
//{
//    std::cout<<"move: "<< other._id <<std::endl;
//    address_t sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
//    sigsetjmp(_env, 1);
//    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
//    (_env->__jmpbuf)[JB_PC] = other._env->__jmpbuf[JB_PC];
//    sigemptyset(&_env->__saved_mask);
//    _state = other._state;
//    _num_quantum = other._num_quantum;
//}
