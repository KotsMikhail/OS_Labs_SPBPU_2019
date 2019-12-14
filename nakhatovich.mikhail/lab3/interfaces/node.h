#ifndef NODE_H
#define NODE_H

#include <pthread.h>

#include <memory>

template<class t>
class node_t
{
public:
    node_t(const t &item);
    ~node_t();

    int lock();
    int unlock();

    t item;
    std::shared_ptr<node_t<t>> next;

private:
    node_t() = delete;
    node_t(node_t const&) = delete;
    node_t& operator=(node_t const&) = delete;

    pthread_mutex_t _lock;
};

#include "node.hpp"

#endif // NODE_H
