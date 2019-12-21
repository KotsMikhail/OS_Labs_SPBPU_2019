//
// Created by misha on 20.12.2019.
//

#include "Tester.h"
#include "Timer.h"
#include "BlockedStack.h"
#include "NonBlockedStack.h"

TestData Tester::DoStackTest(IStack* stack, int R_num, int W_num, int N, int repeat, bool is_need_print) {
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
    return data;
}

void Tester::DoTest(int R_num, int W_num, int N, int repeat, bool is_need_print) {
    std::string delimiter = "-------------------------------------------------------------------------";
    std::string title = "                                One-time                                 ";
    std::string block_str      = "block        ";
    std::string non_block_str  = "non_block    ";
    std::string stack_type_str = "stack_type   ";

    std::cout << delimiter << std::endl;
    std::cout << title << std::endl;
    std::cout << delimiter << std::endl;

    IStack *my_block_stack = new BlockedStack;
    IStack *my_non_block_stack = new NonBlockedStack;
    int stack_num = 2;
    IStack *stacks[stack_num];
    stacks[0] = my_block_stack;
    stacks[1] = my_non_block_stack;
    TestData data;

    std::cout << stack_type_str;
    PrintDataHeader();
    for (int k = 0; k < stack_num; k++) {
        data = DoStackTest(stacks[k], R_num, W_num, N, repeat, is_need_print);
        if(k == 0)
            std::cout << block_str;
        if(k == 1)
            std::cout << non_block_str;
        PrintData(data);
    }
    delete my_block_stack;
    delete my_non_block_stack;
}


void PrintCommonTestData(int min_Rnum, int max_Rnum, int min_Wnum, int max_Wnum, int N, bool amount_result, double mean_time, int repeat) {

    std::string Rnum_str = "[" + std::to_string(min_Rnum) + ", " + std::to_string(max_Rnum) + "]";
    std::string Wnum_str = "[" + std::to_string(min_Wnum) + ", " + std::to_string(max_Wnum) + "]";

    std::cout << GetCeil(Rnum_str.c_str()) << GetCeil(Wnum_str.c_str()) << GetCeil(N) << GetCeil(amount_result) << GetCeil(mean_time) << GetCeil(repeat) << std::endl;
}

void Tester::DoIterTest(int N, int max_thread, int repeat, bool is_need_print) {

    IStack *my_block_stack = new BlockedStack;
    IStack *my_non_block_stack = new NonBlockedStack;
    int stack_num = 2;
    IStack *stacks[stack_num];
    stacks[0] = my_block_stack;
    stacks[1] = my_non_block_stack;

    std::string delimiter = "-------------------------------------------------------------------------";
    std::string R_title   = "                               Reader Test                               ";
    std::string W_title   = "                               Writer Test                               ";
    std::string C_title   = "                               Common Test                               ";
    std::string block_str      = "block        ";
    std::string non_block_str  = "non_block    ";
    std::string stack_type_str = "stack_type   ";

    TestData data;
    int k, i, j;

    // тест читателей
    std::cout << std::endl;
    std::cout << delimiter << std::endl;
    std::cout << R_title << std::endl;
    std::cout << delimiter << std::endl;
    std::cout << stack_type_str;
    PrintDataHeader();
    for (k = 0; k < stack_num; k++) {
        data = DoStackTest(stacks[k], max_thread, 0, N, repeat, is_need_print);
        if(k == 0)
            std::cout << block_str;
        if(k == 1)
            std::cout << non_block_str;
        PrintData(data);
    }
    // тест писателей
    std::cout << std::endl;
    std::cout << delimiter << std::endl;
    std::cout << W_title << std::endl;
    std::cout << delimiter << std::endl;
    std::cout << stack_type_str;
    PrintDataHeader();
    for (k = 0; k < stack_num; k++) {
        data = DoStackTest(stacks[k], 0, max_thread, N, repeat, is_need_print);
        if(k == 0)
            std::cout << block_str;
        if(k == 1)
            std::cout << non_block_str;
        PrintData(data);
    }
    // общий тест c единоразовым выводом
    int W_num;
    std::cout << std::endl;
    std::cout << delimiter << std::endl;
    std::cout << C_title << std::endl;
    std::cout << delimiter << std::endl;
    double amount_time = 0;
    double amount_result = true;
    int count = 0;
    std::cout << stack_type_str;
    PrintDataHeader();
    for (k = 0; k < stack_num; k++) {

        for (i = 1; i < max_thread; i++) {
            W_num = max_thread - i;
            for (j = 1; j <= W_num; j++) {
                data = DoStackTest(stacks[k], i, j, N, repeat, is_need_print);
                amount_time += data.time;
                amount_result = amount_result && data.result;
                count++;
            }
        }
        if(k == 0)
            std::cout << block_str;
        if(k == 1)
            std::cout << non_block_str;
        PrintCommonTestData(1, max_thread-1, 1, max_thread-1, N, amount_result, amount_time/count, repeat);
    }



    delete my_block_stack;
    delete my_non_block_stack;
}
