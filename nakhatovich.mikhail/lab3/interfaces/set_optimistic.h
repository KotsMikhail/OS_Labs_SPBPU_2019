#ifndef SET_OPTIMISTIC_H
#define SET_OPTIMISTIC_H

#include <vector>

#include "set.h"

template<class t, class limits = limits_t<t>, class compare = std::less<t>>
class set_optimistic_t : public set_t<t, limits, compare>
{
public:
    static set_optimistic_t<t, limits, compare> * create_set();

    ~set_optimistic_t();

    bool add(const t &item);
    bool remove(const t &item);
    bool contains(const t &item);

private:
    set_optimistic_t(typename set_t<t, limits, compare>::node_t *head);

    bool validate(typename set_t<t, limits, compare>::node_t *pred, typename set_t<t, limits, compare>::node_t *curr);
    void loop(typename set_t<t, limits, compare>::node_t *&pred, typename set_t<t, limits, compare>::node_t *&curr, const t &item);

    std::vector<typename set_t<t, limits, compare>::node_t*> _removed;
    pthread_mutex_t _lock;
};

#include "set_optimistic.hpp"

#endif // SET_OPTIMISTIC_H
