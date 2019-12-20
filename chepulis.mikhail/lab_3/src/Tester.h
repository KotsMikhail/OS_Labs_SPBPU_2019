//
// Created by misha on 20.12.2019.
//

#ifndef LAB_3_STACK__TESTER_H
#define LAB_3_STACK__TESTER_H
#include "IStack.h"
#include "SupremeTester.h"

class Tester {

    SupremeTester S_tester;
public:
    Tester() {}

    ~Tester() {}

    void DoStackTest(IStack *stack, int R_num, int W_num, int N, int repeat, bool is_need_print);

    void DoTest(int R_num, int W_num, int N, int repeat, bool is_need_print = false);

    void DoIterTest(int N, int max_thread, int repeat, bool is_need_print = false);

};


#endif //LAB_3_STACK__TESTER_H
