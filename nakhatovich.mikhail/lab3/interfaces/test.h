#ifndef TEST_H
#define TEST_H

#include "types.h"

enum set_type_t
{
    SET_FINE = 0,
    SET_OPTIMISTIC
};

void run_writers_test(set_type_t type, size_t cnt_writers, const vector_size_t &cnt_records);
void run_readers_test(set_type_t type, size_t cnt_readers, const vector_size_t &cnt_readings);
void run_common_test(set_type_t type, size_t cnt_readers, size_t cnt_writers, size_t cnt_elements);

#endif // TEST_H