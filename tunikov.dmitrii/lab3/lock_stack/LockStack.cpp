//
// Created by dmitrii on 14.12.2019.
//

#include "LockStack.h"
#include "../exceptions/TimeoutException.h"
#include <stdexcept>

int Stack::m_read_timeout;

LockStack* LockStack::make() {
    pthread_mutex_t m;

    int rc = pthread_mutex_init(&m, nullptr);
    if (rc != 0) {
        throw std::runtime_error("can't init mutex");
    }

    return new LockStack(m);
}

LockStack::LockStack(pthread_mutex_t& mutex){
    m_mutex = mutex;
}

void LockStack::push(const int &val) {
    pthread_mutex_lock(&m_mutex);

    m_data.push_back(val);

    pthread_mutex_unlock(&m_mutex);
}

std::shared_ptr<int> LockStack::pop() {
    timed_lock();

    if (m_data.empty())
        return  std::shared_ptr<int>();

    std::shared_ptr<int> val = std::make_shared<int>(m_data[m_data.size() - 1]);
    m_data.erase(m_data.begin() + m_data.size() - 1);

    pthread_mutex_unlock(&m_mutex);
    return val;
}

void LockStack::timed_lock() {
    struct timespec cur_time{};
    struct timespec start_wait_time{};
    clock_gettime(CLOCK_REALTIME, &start_wait_time);

    while (true)
    {
        int rc = pthread_mutex_trylock(&m_mutex);

        if (rc == EBUSY)
        {
            clock_gettime(CLOCK_REALTIME, &cur_time);
            if (cur_time.tv_sec - start_wait_time.tv_sec > m_read_timeout){
                pthread_yield();
                continue;
            }
        }
        else
        {
            if (rc != 0)
                throw TimeoutException("ERROR: pthread_mutex_trylock return unknown error");
            return;
        }
    }
}

bool LockStack::empty() {
    timed_lock();

    bool res = m_data.empty();

    pthread_mutex_unlock(&m_mutex);
    return res;
}

LockStack::LockStack() = default;
LockStack::~LockStack()
{
    m_data.clear();
    pthread_mutex_destroy(&m_mutex);
}