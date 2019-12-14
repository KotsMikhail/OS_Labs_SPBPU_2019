//
// Created by dmitrii on 14.12.2019.
//

#include "BlockingStack.h"
#include "../exceptions/TimeoutException.h"
#include <stdexcept>
#include <iostream>

int Stack::m_read_timeout;

BlockingStack BlockingStack::make() {
    pthread_mutex_t m;

    int rc = pthread_mutex_init(&m, nullptr);
    if (rc != 0) {
        throw std::runtime_error("can't init mutex");
    }

    return BlockingStack(m);
}

BlockingStack::BlockingStack(pthread_mutex_t& mutex){
    m_mutex = mutex;
}

void BlockingStack::push(const int &val) {
    pthread_mutex_lock(&m_mutex);

    m_data.push_back(val);

    pthread_mutex_unlock(&m_mutex);
}

void BlockingStack::pop(int &val) {
    timed_lock();

    if (m_data.empty())
        throw std::runtime_error("ERROR: can't pop from empty stack");

    val = m_data[m_data.size() - 1];
    m_data.erase(m_data.begin() + m_data.size() - 1);

    pthread_mutex_unlock(&m_mutex);
}

void BlockingStack::timed_lock() {
    struct timespec cur_time{};
    struct timespec start_wait_time{};
    clock_gettime(CLOCK_REALTIME, &start_wait_time);

    while (pthread_mutex_trylock(&m_mutex)){
        clock_gettime(CLOCK_REALTIME, &cur_time);

        if (cur_time.tv_sec - start_wait_time.tv_sec > m_read_timeout)
            throw TimeoutException("ERROR: too long wait time while reading");
    }
}

bool BlockingStack::empty() {
    timed_lock();

    bool res = m_data.empty();

    pthread_mutex_unlock(&m_mutex);
    return res;
}

BlockingStack::BlockingStack() = default;
BlockingStack::~BlockingStack()
{
    m_data.clear();
    pthread_mutex_destroy(&m_mutex);
}

BlockingStack::BlockingStack(const BlockingStack &h) {

}
