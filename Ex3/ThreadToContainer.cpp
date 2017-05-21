//
// Created by yohai.magen on 5/9/17.
//

#include <iostream>
#include "ThreadToContainer.h"

#define DES_ERR "MapReduceFramework Failure: pthread_destroy failed."

ThreadToContainer::ThreadToContainer()
{
}

ThreadToContainer::ThreadToContainer(pthread_t *ithread, MAP_ITEM_VEC container, int read_pos, bool thread_complete):
    _ithread(ithread),
    _container(container),
    _read_pos(read_pos),
    _thread_complete(thread_complete)
{
    _cont_mutex = PTHREAD_MUTEX_INITIALIZER;
}

ThreadToContainer::ThreadToContainer(ThreadToContainer &other):
        ThreadToContainer(other._ithread, other._container, other._read_pos)
{
}

ThreadToContainer& ThreadToContainer::operator=(const ThreadToContainer &other)
{
    this->_ithread = other._ithread;
    this->_container = other._container;
    this->_read_pos = other._read_pos;
    this->_thread_complete = other._thread_complete;
    return *this;
}

ThreadToContainer::~ThreadToContainer()
{
    if(pthread_mutex_destroy(&_cont_mutex))
    {
        std::cerr << DES_ERR << std::endl;
    }
}