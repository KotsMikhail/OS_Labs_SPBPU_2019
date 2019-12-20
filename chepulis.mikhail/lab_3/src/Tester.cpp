//
// Created by misha on 20.12.2019.
//

#include "Tester.h"
#include "TestData.h"
#include "Timer.h"
#include "BlockedStack.h"
#include "NonBlockedStack.h"

void Tester::DoStackTest(IStack* stack, int R_num, int W_num, int N, int repeat, bool is_need_print) {
    Timer timer;
    bool test_res = true;
    bool amount_test_res = true;
    TestData data;
    double amount_time = 0;

    for (int r = 0; r < repeat; r++) {
        test_res = S_tester.Test(stack, R_num, W_num, N, &timer, is_need_print);
        amount_test_res = amount_test_res && test_res;
        amount_time += timer.GetTime();
    }
    data = TestData(R_num, W_num, N, amount_test_res, amount_time / repeat, repeat);
    PrintData(data);
}

void Tester::DoTest(int R_num, int W_num, int N, int repeat, bool is_need_print) {
    std::string delimiter = "-------------------------------------------------------------------------";
    std::string title = "                                One-time                                 ";

    std::cout << delimiter << std::endl;
    std::cout << title << std::endl;
    std::cout << delimiter << std::endl;

    IStack *my_block_stack = new BlockedStack;
    IStack *my_non_block_stack = new NonBlockedStack;
    int stack_num = 2;
    IStack *stacks[stack_num];
    stacks[0] = my_block_stack;
    stacks[1] = my_non_block_stack;

    for (int k = 0; k < stack_num; k++) {
        PrintTitle(k);
        PrintDataHeader();
        DoStackTest(stacks[k], R_num, W_num, N, repeat, is_need_print);
    }
    delete my_block_stack;
    delete my_non_block_stack;
}

void Tester::DoIterTest(int N, int max_thread, int repeat, bool is_need_print) {

    IStack *my_block_stack = new BlockedStack;
    IStack *my_non_block_stack = new NonBlockedStack;
    int stack_num = 2;
    IStack *stacks[stack_num];
    stacks[0] = my_block_stack;
    stacks[1] = my_non_block_stack;

    std::string delimiter = "-------------------------------------------------------------------------";
    std::string R_title = "                               Reader Test                               ";
    std::string W_title = "                               Writer Test                               ";
    std::string C_title = "                               Common Test                               ";

    int k, i, j;

    std::cout << delimiter << std::endl;
    std::cout << R_title << std::endl;
    std::cout << delimiter << std::endl;
    for (k = 0; k < stack_num; k++) {
        PrintTitle(k);
        PrintDataHeader();
        for (i = 1; i <= max_thread; i++) {
            DoStackTest(stacks[k], i, 0, N, repeat, is_need_print);
        }
    }
    std::cout << delimiter << std::endl;
    std::cout << W_title << std::endl;
    std::cout << delimiter << std::endl;
    for (k = 0; k < stack_num; k++) {
        PrintTitle(k);
        PrintDataHeader();
        for (i = 1; i <= max_thread; i++) {
            DoStackTest(stacks[k], 0, i, N, repeat, is_need_print);
        }
    }
    int W_num;
    std::cout << delimiter << std::endl;
    std::cout << C_title << std::endl;
    std::cout << delimiter << std::endl;
    for (k = 0; k < stack_num; k++) {
        PrintTitle(k);
        PrintDataHeader();
        for (i = 1; i < max_thread; i++) {
            W_num = max_thread - i;
            for (j = 1; j <= W_num; j++) {
                DoStackTest(stacks[k], i, j, N, repeat, is_need_print);
            }
        }
    }

    delete my_block_stack;
    delete my_non_block_stack;
}
