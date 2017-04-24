#include <iostream>
#include "thread.h"

thread::thread() : thread(-1 , 0, blockedNwaiting) {}

thread::thread(unsigned int id, address_t pc, threadState state, unsigned int quantum)
{
    std::cout<<"constructor: "<< id <<std::endl;
    _id = id;
    address_t sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
    sigsetjmp(_env, 1);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&_env->__saved_mask);
    _state = state;
    _num_quantum = quantum;
}

thread::thread(const thread& other) //: thread(other._id, other._env->__jmpbuf[JB_PC], other._state, other._num_quantum)
{
    std::cout<<"copy: "<< other._id <<std::endl;
    _id = other._id;
    address_t sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
    sigsetjmp(_env, 1);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = other._env->__jmpbuf[JB_PC];
    sigemptyset(&_env->__saved_mask);
    _state = other._state;
    _num_quantum = other._num_quantum;
}

thread::~thread() {}

thread thread::operator=(const thread &other) {
    std::cout<<"operator="<<std::endl;
    address_t sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
    sigsetjmp(_env, 1);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = other._env->__jmpbuf[JB_PC];
    sigemptyset(&_env->__saved_mask);
    _state = other._state;
    _num_quantum = other._num_quantum;
    return *this;
}
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
