#ifndef SET_FINE_H
#define SET_FINE_H

#include "set.h"

template<class t, class limits = limits_t<t>, class compare = std::less<t>>
class set_fine_t : public set_t<t, limits, compare>
{
public:
    static set_fine_t<t, limits, compare> * create_set();

    bool add(const t &item);
    bool remove(const t &item);
    bool contains(const t &item);

private:
    set_fine_t(typename set_t<t, limits, compare>::node_t *head);

    void loop(typename set_t<t, limits, compare>::node_t *&pred, typename set_t<t, limits, compare>::node_t *&curr, const t &item);
};

#include "set_fine.hpp"

#endif // SET_FINE_H
