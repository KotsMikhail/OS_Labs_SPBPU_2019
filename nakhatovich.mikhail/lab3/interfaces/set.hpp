#include "set.h"

template<class t, class l, class c>
set_t<t, l, c>::set_t(node_t *head) : _head(head)
{}

template<class t, class l, class c>
typename set_t<t, l, c>::node_t * set_t<t, l, c>::create_head()
{
    l limits;
    t min = limits.min(), max = limits.max();
    node_t * head = new (std::nothrow) node_t(min);
    if (!head)
        return nullptr;
    head->next = new (std::nothrow) node_t(max);
    if (!head->next)
    {
        delete head;
        return nullptr;
    }
    return head;
}

template<class t, class l, class c>
bool set_t<t, l, c>::empty()
{
    l limits;
    t min = limits.min(), max = limits.max();
    node_t *next = _head->next;
    return (!_cmp(_head->item, min) && !_cmp(min, _head->item) && !_cmp(next->item, max) && !_cmp(max, next->item));
}

template<class t, class l, class c>
void set_t<t, l, c>::unlock(node_t *pred, node_t *curr)
{
    pred->unlock();
    curr->unlock();
}

template<class t, class l, class c>
set_t<t, l, c>::~set_t()
{
    node_t *to_del;
    while (_head)
    {
        to_del = _head;
        _head = _head->next;
        delete to_del;
    }
}

template<class t, class l, class c>
set_t<t, l, c>::node_t::node_t(const t &item) : item(item), next(nullptr), _lock(PTHREAD_MUTEX_INITIALIZER)
{}

template<class t, class l, class c>
set_t<t, l, c>::node_t::~node_t()
{
    pthread_mutex_destroy(&_lock);
}

template<class t, class l, class c>
int set_t<t, l, c>::node_t::lock()
{
    return pthread_mutex_lock(&_lock);
}

template<class t, class l, class c>
int set_t<t, l, c>::node_t::unlock()
{
    return pthread_mutex_unlock(&_lock);
}
