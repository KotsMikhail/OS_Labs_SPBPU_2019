#ifndef SET_H
#define SET_H

#include <functional>

#include "min_max.h"
#include "node.h"

template<class t, class limits = limits_t<t>, class compare = std::less<t>>
class set_t
{
public:
    virtual ~set_t() = default;

    virtual bool add(const t &item) = 0;
    virtual bool remove(const t &item) = 0;
    virtual bool contains(const t &item) = 0;

protected:
    set_t() = delete;
    set_t(set_t const&) = delete;
    set_t& operator=(set_t const&) = delete;

    set_t(node_t<t> *head);

    static node_t<t> * create_head();

    std::shared_ptr<node_t<t>> _head;
    compare _cmp;
};

#include "set.hpp"

#endif // SET_H
