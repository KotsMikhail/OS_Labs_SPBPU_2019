#include "set_optimistic.h"

// check if node wasn't remove
#define check_opt(node) \
((size_t)node > 0xffff)

#define loop_opt(pred, curr, _cmp, item) \
while (check_opt(curr) && _cmp(curr->item, item)) \
{ \
    pred = curr; \
    if (check_opt(pred)) \
        curr = pred->next; \
    else \
        break; \
} \
if (check_opt(pred)) \
    pred->lock(); \
else \
    continue; \
if (check_opt(curr)) \
    curr->lock(); \
else \
{ \
    pred->unlock(); \
    continue; \
}

#define unlock_opt(pred, curr) \
pred->unlock(); \
curr->unlock();

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
{}

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::add(const t &item)
{
    bool ret = false;
    while (true)
    {
        node_t<t> *pred = this->_head, *curr = pred->next;
        loop_opt(pred, curr, this->_cmp, item)
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
            unlock_opt(pred, curr)
            break;
        }
        unlock_opt(pred, curr)
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
        loop_opt(pred, curr, this->_cmp, item)
        if (validate(pred, curr))
        {
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
            break;
        }
        unlock_opt(curr, pred)
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
        loop_opt(pred, curr, this->_cmp, item)
        if (validate(pred, curr))
        {
            ret = !this->_cmp(item, curr->item);
            unlock_opt(pred, curr)
            return ret;
        }
        unlock_opt(pred, curr)
    }
    return ret;
}

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::validate(node_t<t> *pred, node_t<t> *curr)
{
    node_t<t> *node = this->_head;
    while (check_opt(node) && this->_cmp(node->item, pred->item))
        node = node->next;
    return (node == pred && pred->next == curr);
}
