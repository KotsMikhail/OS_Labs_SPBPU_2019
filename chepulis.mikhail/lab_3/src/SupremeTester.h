//
// Created by misha on 19.12.2019.
//

#ifndef LAB_3_STACK__SUPREMETESTER_H
#define LAB_3_STACK__SUPREMETESTER_H


#include "IStack.h"
#include "Timer.h"
#include "CommonTester.h"
#include "ReaderTester.h"
#include "WriterTester.h"

class SupremeTester {

private:
    CommonTester C_tester;
    ReaderTester R_tester;
    WriterTester W_tester;
public:
    SupremeTester() {}

    ~SupremeTester() {}

    bool Test(IStack *my_stack, int num_of_reader, int num_of_writer, int num_of_elements, Timer *timer = nullptr,
              bool is_need_print = false);
};


#endif //LAB_3_STACK__SUPREMETESTER_H
