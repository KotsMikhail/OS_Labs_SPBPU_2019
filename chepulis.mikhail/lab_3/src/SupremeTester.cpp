//
// Created by misha on 19.12.2019.
//

#include "SupremeTester.h"

bool SupremeTester::Test(IStack *my_stack, int num_of_reader, int num_of_writer, int num_of_elements, Timer *timer, bool is_need_print) {


    if (num_of_reader == 0) {
        return W_tester.Test(my_stack, num_of_writer, num_of_elements, timer, is_need_print);
    }

    if (num_of_writer == 0) {
        return R_tester.Test(my_stack, num_of_reader, num_of_elements, timer, is_need_print);
    }
    return C_tester.Test(my_stack, num_of_reader, num_of_writer, num_of_elements, timer, is_need_print);
}
