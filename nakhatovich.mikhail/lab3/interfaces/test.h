#ifndef TEST_H
#define TEST_H

#include "types.h"

enum set_type_t
{
    SET_FINE = 0,
    SET_OPTIMISTIC
};

enum simple_test_type_t
{
    TEST_WRITERS = 0,
    TEST_READERS,
    TEST_COMMON
};

void run_simple_test(set_type_t set_type, simple_test_type_t test_type);
void run_time_test();

#endif // TEST_H
