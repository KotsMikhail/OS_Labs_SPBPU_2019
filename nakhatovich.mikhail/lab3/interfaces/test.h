#ifndef TEST_H
#define TEST_H

#include "types.h"

enum set_type_t
{
    SET_FINE = 0,
    SET_OPTIMISTIC
};

void run_writers_test(set_type_t type);
void run_readers_test(set_type_t type);
void run_common_test(set_type_t type);
void run_time_test();

#endif // TEST_H