#include "thread.h"

thread::thread() : thread(-1 , 0, blockedNwaiting) {}

thread::thread(unsigned int id, address_t pc, threadState state)
{
    _id = id;
    address_t sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = translate_address(pc);
    _state = state;
    _num_quantum = 0;
}

thread::~thread() {}


