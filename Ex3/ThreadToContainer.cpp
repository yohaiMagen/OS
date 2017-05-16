//
// Created by yohai.magen on 5/9/17.
//

#include "ThreadToContainer.h"

ThreadToContainer::ThreadToContainer()
{
}

ThreadToContainer::ThreadToContainer(pthread_t *ithread, MAP_ITEM_VEC container, int read_pos, bool thread_done):
    _ithread(ithread),
    _container(container),
    _read_pos(read_pos),
    _thread_done(thread_done)
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
    this->_thread_done = other._thread_done;
    return *this;
}

ThreadToContainer::~ThreadToContainer()
{
    if(pthread_mutex_destroy(&_cont_mutex))
    {
        //TODO err
    }
}