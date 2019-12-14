#include "set_optimistic.h"

#define loop_opt(pred, curr, _cmp, item) \
while (_cmp(curr->item, item)) \
{ \
    pred = curr; \
    curr = pred->next; \
} \
pred->lock(); \
curr->lock();

template<class t, class l, class c>
set_optimistic_t<t, l, c> * set_optimistic_t<t, l, c>::create_set()
{
    node_t<t> *head = set_t<t, l, c>::create_head();
    if (!head)
        return nullptr;
    set_optimistic_t<t, l, c> * set = new (std::nothrow) set_optimistic_t<t, l, c>(head);
    if (!set)
        delete head;
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
        std::shared_ptr<node_t<t>> pred = this->_head, curr = pred->next;
        loop_opt(pred, curr, this->_cmp, item)
        if (validate(pred, curr))
        {
            if (this->_cmp(item, curr->item))
            {
                node_t<t> *node = new (std::nothrow) node_t<t>(item);
                if (node)
                {
                    node->next = curr;
                    pred->next = std::shared_ptr<node_t<t>>(node);
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
        std::shared_ptr<node_t<t>> pred = this->_head, curr = pred->next;
        loop_opt(pred, curr, this->_cmp, item)
        if (validate(pred, curr))
        {
            if (!this->_cmp(item, curr->item))
            {
                pred->next = curr->next;
                ret = true;
            }
            curr->unlock();
            pred->unlock();
            return ret;
        }
        curr->unlock();
        pred->unlock();

    }
    return ret;
}

template<class t, class l, class c>
bool set_optimistic_t<t, l, c>::contains(const t &item)
{
    bool ret = false;
    while (true)
    {
        std::shared_ptr<node_t<t>> pred = this->_head, curr = pred->next;
        loop_opt(pred, curr, this->_cmp, item)
        if (validate(pred, curr))
        {
            ret = !this->_cmp(item, curr->item);
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
bool set_optimistic_t<t, l, c>::validate(std::shared_ptr<node_t<t>> &pred, std::shared_ptr<node_t<t>> &curr)
{
    std::shared_ptr<node_t<t>> node = this->_head;
    while (this->_cmp(node->item, pred->item)) 
        node = node->next;
    if (node == pred)
        return (pred->next == curr);
    return false;
}
