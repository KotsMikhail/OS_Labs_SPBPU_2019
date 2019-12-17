#include "set_optimistic.h"

template<class t, class l, class c>
set_optimistic_t<t, l, c> * set_optimistic_t<t, l, c>::create_set()
{
    typename set_t<t, l, c>::node_t *head = set_t<t, l, c>::create_head();
    if (!head)
        return nullptr;
    set_optimistic_t<t, l, c> * set = new (std::nothrow) set_optimistic_t<t, l, c>(head);
    if (!set)
    {
        delete head->next;
        delete head;
    }
    return set;
}

template<class t, class l, class c>
set_optimistic_t<t, l, c>::set_optimistic_t(typename set_t<t, l, c>::node_t *head) : set_t<t, l, c>(head), _lock(PTHREAD_MUTEX_INITIALIZER)
{}

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::add(const t &item)
{
    bool ret = false;
    while (true)
    {
        typename set_t<t, l, c>::node_t *pred = this->_head, *curr = pred->next;
        loop(pred, curr, item);
        if (validate(pred, curr))
        {
            if (this->_cmp(item, curr->item))
            {
                typename set_t<t, l, c>::node_t *node = new (std::nothrow) typename set_t<t, l, c>::node_t(item);
                if (node)
                {
                    node->next = curr;
                    pred->next = node;
                    ret = true;
                }
            }
            this->unlock(pred, curr);
            break;
        }
        this->unlock(pred, curr);
    }
    return ret;
}

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::remove(const t &item)
{
    bool ret = false;
    while (true)
    {
        typename set_t<t, l, c>::node_t *pred = this->_head, *curr = pred->next;
        loop(pred, curr, item);
        if (validate(pred, curr))
        {
            if (!this->_cmp(item, curr->item))
            {
                pred->next = curr->next;
                pthread_mutex_lock(&_lock);
                _removed.push_back(curr);
                pthread_mutex_unlock(&_lock);
                ret = true;
            }
            this->unlock(curr, pred);
            break;
        }
        this->unlock(curr, pred);
    }
    return ret;
}

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::contains(const t &item)
{
    bool ret = false;
    while (true)
    {
        typename set_t<t, l, c>::node_t *pred = this->_head, *curr = pred->next;
        loop(pred, curr, item);
        if (validate(pred, curr))
        {
            ret = !this->_cmp(item, curr->item);
            this->unlock(pred, curr);
            break;
        }
        this->unlock(pred, curr);
    }
    return ret;
}

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::validate(typename set_t<t, l, c>::node_t *pred, typename set_t<t, l, c>::node_t *curr)
{
    typename set_t<t, l, c>::node_t *node = this->_head;
    while (this->_cmp(node->item, pred->item))
        node = node->next;
    return (node == pred && pred->next == curr);
}

template<class t, class l, class c>
void set_optimistic_t<t, l, c>::loop(typename set_t<t, l, c>::node_t *&pred, typename set_t<t, l, c>::node_t *&curr, const t &item)
{
    while (this->_cmp(curr->item, item))
    {
        pred = curr;
        curr = pred->next;
    }
    pred->lock();
    curr->lock();
}

template<class t, class l, class c>
set_optimistic_t<t, l, c>::~set_optimistic_t()
{
    for (typename set_t<t, l, c>::node_t *to_del : _removed)
        delete to_del;
    pthread_mutex_destroy(&_lock);
}
