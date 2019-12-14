#include "set_optimistic.h"

template<class t, class l, class c>
set_optimistic_t<t, l, c> * set_optimistic_t<t, l, c>::create_set()
{
    node_t<t> *head = set_t<t, l, c>::create_head();
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
set_optimistic_t<t, l, c>::set_optimistic_t(node_t<t> *head) : set_t<t, l, c>(head)
{};

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::add(const t &item)
{
    bool ret = false;
    while (true)
    {
        node_t<t> *pred = this->_head, *curr = pred->next;
        while (this->_cmp(curr->item, item))
        {
            pred = curr;
            curr = pred->next;
        }
        pred->lock();
        curr->lock();
        if (validate(pred, curr))
        {
            if (this->_cmp(item, curr->item))
            {
                node_t<t> *node = new (std::nothrow) node_t<t>(item);
                if (node)
                {
                    node->next = curr;
                    pred->next = node;
                    ret = true;
                }
            }
            pred->unlock();
            curr->unlock();
            return ret;
        }
        pred->unlock();
        curr->unlock();
    }
    return ret;
}

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::remove(const t &item)
{
    bool ret = false;
    while (true)
    {
        node_t<t> *pred = this->_head, *curr = pred->next;
        while (this->_cmp(curr->item, item))
        {
            pred = curr;
            curr = pred->next;
        }
        pred->lock();
        curr->lock();
        if (validate(pred, curr))
        {
            if (!this->_cmp(item, curr->item))
            {
                pred->next = curr->next;
                delete curr;
                ret = true;
            }
            else
                curr->unlock();
            pred->unlock();
            return ret;
        }
        pred->unlock();
        curr->unlock();
    }
    return ret;
}

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::contains(const t &item)
{
    bool ret = false;
    while (true)
    {
        node_t<t> *pred = this->_head, *curr = pred->next;
        while (this->_cmp(curr->item, item))
        {
            pred = curr;
            curr = pred->next;
        }
        pred->lock();
        curr->lock();
        if (validate(pred, curr))
        {
            if (!this->_cmp(item, curr->item))
                ret = true;
            pred->unlock();
            curr->unlock();
            return ret;
        }
        pred->unlock();
        curr->unlock();
    }
    return ret;
}

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::validate(node_t<t> *pred, node_t<t> *curr)
{
    node_t<t> *node = this->_head;
    while (!this->_cmp(pred->item, node->item)) 
    {
        if (node == pred)
            return (pred->next == curr);
        node = node->next;
    }
    return false;
}