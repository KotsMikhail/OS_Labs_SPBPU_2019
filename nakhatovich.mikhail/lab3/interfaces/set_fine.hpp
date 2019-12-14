#include "set_fine.h"

#define loop_fine(pred, curr, _cmp, item) \
curr->lock(); \
while (_cmp(curr->item, item)) \
{ \
    pred->unlock(); \
    pred = curr; \
    curr = pred->next; \
    curr->lock(); \
}

template<class t, class l, class c>
set_fine_t<t, l, c> * set_fine_t<t, l, c>::create_set()
{
    node_t<t> *head = set_t<t, l, c>::create_head();
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
set_fine_t<t, l, c>::set_fine_t(node_t<t> *head) : set_t<t, l, c>(head)
{};

template<class t, class l, class c>
bool set_fine_t<t, l, c>::add(const t &item)
{
    bool ret = false;
    this->_head->lock();
    node_t<t> *pred = this->_head, *curr = pred->next;
    loop_fine(pred, curr, this->_cmp, item);
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

template<class t, class l, class c>
bool set_fine_t<t, l, c>::remove(const t &item)
{
    bool ret = false;
    this->_head->lock();
    node_t<t> *pred = this->_head, *curr = pred->next;
    loop_fine(pred, curr, this->_cmp, item);
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

template<class t, class l, class c>
bool set_fine_t<t, l, c>::contains(const t &item)
{
    bool ret = false;
    this->_head->lock();
    node_t<t> *pred = this->_head, *curr = pred->next;
    loop_fine(pred, curr, this->_cmp, item);
    if (!this->_cmp(item, curr->item))
        ret = true;
    pred->unlock();
    curr->unlock();
    return ret;
}
