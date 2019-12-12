#include "set.h"

template<class t, class l, class c>
set_t<t, l, c>::set_t(node_t<t> *head) : _head(head)
{};

template<class t, class l, class c>
node_t<t> * set_t<t, l, c>::create_head()
{
    l limits;
    t min = limits.min(), max = limits.max();
    node_t<t> * head = new (std::nothrow) node_t<t>(min);
    if (!head)
        return nullptr;
    head->next = new (std::nothrow) node_t<t>(max);
    if (!head->next)
    {
        delete head;
        return nullptr;
    }
    return head;
}

template<class t, class l, class c>
set_t<t, l, c>::~set_t()
{
    node_t<t> *to_del;
    while (_head)
    {
        to_del = _head;
        _head = _head->next;
        delete to_del;
    }
}
