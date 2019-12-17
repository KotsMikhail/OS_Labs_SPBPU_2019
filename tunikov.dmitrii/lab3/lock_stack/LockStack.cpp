//
// Created by dmitrii on 14.12.2019.
//

#include "LockStack.h"
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
    head = nullptr;
}

void LockStack::push(const int &val) {
    pthread_mutex_lock(&m_mutex);

    Node* new_elem = new (std::nothrow) Node(val);
    new_elem->m_next = head;
    head = new_elem;

    pthread_mutex_unlock(&m_mutex);
}

std::shared_ptr<int> LockStack::pop() {
    timed_lock();

    if (head == nullptr)
    {
        pthread_mutex_unlock(&m_mutex);
        return  std::shared_ptr<int>();
    }

    Node* old_head = head;
    std::shared_ptr<int> val = std::make_shared<int>(*(old_head->m_data));

    if (old_head->m_next)
        head = old_head->m_next;
    else
        head = nullptr;

    delete old_head;

    pthread_mutex_unlock(&m_mutex);
    return val;
}

LockStack::Node::Node(const int &data) {
    m_data = std::make_shared<int>(data);
}

void LockStack::Node::delete_nodes(Node *node) {
    while (node)
    {
        Node* next = node->m_next;
        delete node;
        node = next;
    }
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
                throw std::runtime_error("ERROR: pthread_mutex_trylock return unknown error");
            return;
        }
    }
}

bool LockStack::empty() {
    timed_lock();

    bool res = head == nullptr;

    pthread_mutex_unlock(&m_mutex);
    return res;
}

LockStack::~LockStack()
{
    Node::delete_nodes(head);
    pthread_mutex_destroy(&m_mutex);
}