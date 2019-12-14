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
    head->next = std::shared_ptr<node_t<t>>(new (std::nothrow) node_t<t>(max));
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
    std::shared_ptr<node_t<t>> next = _head->next;
    return (!_cmp(_head->item, min) && !_cmp(min, _head->item) && !_cmp(next->item, max) && !_cmp(max, next->item));
}
