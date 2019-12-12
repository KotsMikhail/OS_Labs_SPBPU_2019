#include <limits>

#include "min_max.h"

template<class t>
t limits_t<t>::max()
{
    return std::numeric_limits<t>().max();
}

template<class t>
t limits_t<t>::min()
{
    return std::numeric_limits<t>().lowest();
}
