#include <new>

#include "node.h"

template<class t>
node_t<t>::node_t(const t &item) : item(item), next(nullptr), _lock(PTHREAD_MUTEX_INITIALIZER)
{};

template<class t>
node_t<t>::~node_t()
{
    pthread_mutex_destroy(&_lock);
}

template<class t>
int node_t<t>::lock()
{
    return pthread_mutex_lock(&_lock);
}

template<class t>
int node_t<t>::unlock()
{
    return pthread_mutex_unlock(&_lock);
}
