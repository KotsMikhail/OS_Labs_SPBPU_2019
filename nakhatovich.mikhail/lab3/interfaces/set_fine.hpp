#include "set_fine.h"

template<class t, class l, class c>
set_fine_t<t, l, c> * set_fine_t<t, l, c>::create_set()
{
    typename set_t<t, l, c>::node_t *head = set_t<t, l, c>::create_head();
    if (!head)
        return nullptr;
    set_fine_t<t, l, c> * set = new (std::nothrow) set_fine_t<t, l, c>(head);
    if (!set)
    {
        delete head->next;
        delete head;
    }
    return set;
}

template<class t, class l, class c>
set_fine_t<t, l, c>::set_fine_t(typename set_t<t, l, c>::node_t *head) : set_t<t, l, c>(head)
{}

template<class t, class l, class c>
bool set_fine_t<t, l, c>::add(const t &item)
{
    bool ret = false;
    this->_head->lock();
    typename set_t<t, l, c>::node_t *pred = this->_head, *curr = pred->next;
    loop(pred, curr, item);
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
    return ret;
}

template<class t, class l, class c>
bool set_fine_t<t, l, c>::remove(const t &item)
{
    bool ret = false;
    this->_head->lock();
    typename set_t<t, l, c>::node_t *pred = this->_head, *curr = pred->next;
    loop(pred, curr, item);
    if (!this->_cmp(item, curr->item))
    {
        pred->next = curr->next;
        curr->unlock();
        delete curr;
        ret = true;
    }
    else
        curr->unlock();
    pred->unlock();
    return ret;
}

template<class t, class l, class c>
bool set_fine_t<t, l, c>::contains(const t &item)
{
    bool ret = false;
    this->_head->lock();
    typename set_t<t, l, c>::node_t *pred = this->_head, *curr = pred->next;
    loop(pred, curr, item);
    ret = !this->_cmp(item, curr->item);
    this->unlock(pred, curr);
    return ret;
}

template<class t, class l, class c>
void set_fine_t<t, l, c>::loop(typename set_t<t, l, c>::node_t *&pred, typename set_t<t, l, c>::node_t *&curr, const t &item)
{
    curr->lock();
    while (this->_cmp(curr->item, item))
    {
        pred->unlock();
        pred = curr;
        curr = pred->next;
        curr->lock();
    }
}
