#ifndef SET_OPTIMISTIC_H
#define SET_OPTIMISTIC_H

#include <functional>

#include "set.h"

template<class t, class limits = limits_t<t>, class compare = std::less<t>>
class set_optimistic_t : public set_t<t, limits, compare>
{
public:
    static set_optimistic_t<t, limits, compare> * create_set();

    bool add(const t &item);
    bool remove(const t &item);
    bool contains(const t &item);

private:
    set_optimistic_t(node_t<t> *head);

    bool validate(node_t<t> *pred, node_t<t> *curr);
};

#include "set_optimistic.hpp"

#endif // SET_OPTIMISTIC_H
