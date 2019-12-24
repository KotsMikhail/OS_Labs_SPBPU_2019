#ifndef LAB3_SET_OPTIMISTIC_H
#define LAB3_SET_OPTIMISTIC_H

#include <vector>

#include "set.h"

template<typename T, typename Limits = limits_t<T>>
class SetOptimistic : public Set<T, Limits>
{
public:
    SetOptimistic();

    ~SetOptimistic();

    bool add(const T &item);
    bool remove(const T &item);
    bool contains(const T &item);

private:
    bool validate(typename Set<T, Limits>::Node *pred, typename Set<T, Limits>::Node *curr);

    std::vector<typename Set<T, Limits>::Node*> _removed;
    pthread_mutex_t _mutex;
};

#include "set_optimistic.hpp"

#endif // LAB3_SET_OPTIMISTIC_H
