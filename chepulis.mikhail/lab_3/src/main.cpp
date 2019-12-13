#include <iostream>
#include <unistd.h>

#include "IStack.h"
#include "ReaderTester.h"
#include "WriterTester.h"
#include "BlockedStack.h"
#include "NonBlockedStack.h"
#include "CommonTester.h"
#include "Timer.h"
#include "TestData.h"

#include <sys/resource.h>

int GetNpocLim(){
    struct rlimit lim;
    getrlimit(RLIMIT_NPROC, &lim);
    //std::cout << "cur_lim threads = " << lim.rlim_cur << std::endl;
    return lim.rlim_cur;
}

int main() {
    bool test_res;

    IStack *my_block_stack = new BlockedStack;
    IStack *my_non_block_stack = new NonBlockedStack;
    bool is_need_test[4];
    is_need_test[0] = true; // Тест времени
    is_need_test[1] = true; // Reader test для различных сочетаний кол-ва читателей/писателей
    is_need_test[2] = true; // Writer test для различных сочетаний кол-ва читателей/писателей
    is_need_test[3] = true; // Common test для различных сочетаний кол-ва читателей/писателей

    Timer timer;
    ReaderTester R_tester;
    WriterTester W_tester;
    CommonTester C_tester;
    std::cout << std::boolalpha;

    int N, R_num, W_num;
    int max_thread = 25;
    int nproc_lim = GetNpocLim();
    if (nproc_lim < max_thread) {
        max_thread = nproc_lim;
    }
    N = 10000;
    int stack_num = 2;
    int repeat = 10;

    if (is_need_test[0]) {

        R_num = max_thread/2;
        W_num = max_thread/2;
        int local_N;
        //local_N = N;
        local_N = 1000000;
        std::cout << "(" << R_num << ", " << W_num << ", " << local_N << ")" << std::endl;
        test_res = R_tester.Test(my_block_stack, R_num, local_N, &timer);
        std::cout << "block\t\tReader test : " << test_res << "\ttime = " << timer.GetTime() << " sec" << std::endl;

        test_res = W_tester.Test(my_block_stack, W_num, local_N, &timer);
        std::cout << "block\t\tWriter test : " << test_res << "\ttime = " << timer.GetTime() << " sec" << std::endl;

        test_res = C_tester.Test(my_block_stack, R_num, W_num, local_N, &timer);
        std::cout << "block\t\tCommon test : " << test_res << "\ttime = " << timer.GetTime() << " sec" << std::endl;


        test_res = R_tester.Test(my_non_block_stack, R_num, local_N, &timer);
        std::cout << "non block\tReader test : " << test_res << "\ttime = " << timer.GetTime() << " sec" << std::endl;

        test_res = W_tester.Test(my_non_block_stack, W_num, local_N, &timer);
        std::cout << "non block\tWriter test : " << test_res << "\ttime = " << timer.GetTime() << " sec" << std::endl;

        test_res = C_tester.Test(my_non_block_stack, R_num, W_num, local_N, &timer);
        std::cout << "non block\tCommon test : " << test_res << "\ttime = " << timer.GetTime() << " sec" << std::endl;
    }



    IStack *stacks[stack_num];
    stacks[0] = my_block_stack;
    stacks[1] = my_non_block_stack;

    std::cout << std::endl << std::endl;

    TestData R_data_matrix[stack_num][max_thread];
    TestData W_data_matrix[stack_num][max_thread];
    TestData C_data_matrix[stack_num][max_thread][max_thread];
    bool test_result[stack_num];


    if (is_need_test[1]) {
        for (int k = 0; k < stack_num; k++) {
            test_result[k] = true;
        }
        for (int k = 0; k < stack_num; k++) {
            for (int j = 1; j < max_thread; j++) {
                double amount_time = 0;
                for (int r = 0; r <= repeat; r++) {
                    test_res = R_tester.Test(stacks[k], j, N, &timer);
                    test_result[k] = test_result[k] && test_res;
                    amount_time += timer.GetTime();
                }
                R_data_matrix[k][j] = TestData(j, 0, N, test_res, amount_time / repeat, repeat);
            }
        }

        std::cout << "R : block    \tRead test : " << test_result[0] << std::endl;
        std::cout << "R : non block\tRead test : " << test_result[1] << std::endl;
/*
        std::cout << "------------------------\t\tReader Test\t\t------------------------" << std::endl;
        for (int k = 0; k < stack_num; k++){
            if(k == 0)
            {
                std::cout << "-------------\t\tBlocked Stack    \t\t-------------" << std::endl;
            }else{
                std::cout << "-------------\t\tNon Blocked Stack\t\t-------------" << std::endl;
            }

            for (int i = 1; i < max_thread; i++){
                PrintData(R_data_matrix[k][i]);
            }
        }
*/
    }

    if (is_need_test[2]) {
        for (int k = 0; k < stack_num; k++) {
            test_result[k] = true;
        }
        for (int k = 0; k < stack_num; k++) {
            for (int i = 1; i < max_thread; i++) {
                double amount_time = 0;
                for (int r = 0; r <= repeat; r++) {
                    test_res = W_tester.Test(stacks[k], i, N, &timer);
                    test_result[k] = test_result[k] && test_res;
                    amount_time += timer.GetTime();
                }
                W_data_matrix[k][i] = TestData(0, i, N, test_res, amount_time / repeat, repeat);

            }
        }

        std::cout << "W : block    \tWriter test : " << test_result[0] << std::endl;
        std::cout << "W : non block\tWriter test : " << test_result[1] << std::endl;
    }

    if (is_need_test[3]) {
        for (int k = 0; k < stack_num; k++) {
            test_result[k] = true;
        }
        for (int k = 0; k < stack_num; k++) {
            for (int i = 1; i < max_thread; i++) {
                R_num = max_thread - i;
                for (int j = 1; j < R_num; j++) {
                    double amount_time = 0;
                    for (int r = 0; r <= repeat; r++) {
                        test_res = C_tester.Test(stacks[k], i, j, N, &timer);
                        test_result[k] = test_result[k] && test_res;
                        amount_time += timer.GetTime();
                    }
                    C_data_matrix[k][i][j] = TestData(i, j, N, test_res, amount_time / repeat, repeat);
                }
            }
        }

        std::cout << "C : block    \tCommon test : " << test_result[0] << std::endl;
        std::cout << "C : non block\tCommon test : " << test_result[1] << std::endl;
    }

    if (is_need_test[1]) {

        std::cout << "-------------------------------------------------------------------------" << std::endl;
        std::cout << "                               Reader Test                               " << std::endl;
        std::cout << "-------------------------------------------------------------------------" << std::endl;
        for (int k = 0; k < stack_num; k++) {
            PrintTitle(k);
            PrintDataHeader();
            for (int i = 1; i < max_thread; i++) {
                PrintData(R_data_matrix[k][i]);
            }
        }
    }
    if (is_need_test[2]) {
        std::cout << "-------------------------------------------------------------------------" << std::endl;
        std::cout << "                               Writer Test                               " << std::endl;
        //std::cout << "------------------------\t\tWriter Test\t\t------------------------" << std::endl;
        std::cout << "-------------------------------------------------------------------------" << std::endl;
        for (int k = 0; k < stack_num; k++) {
            PrintTitle(k);
            PrintDataHeader();
            for (int i = 1; i < max_thread; i++) {
                PrintData(W_data_matrix[k][i]);
            }
        }
    }
    if (is_need_test[3]) {
        std::cout << "-------------------------------------------------------------------------" << std::endl;
        std::cout << "                               Common Test                               " << std::endl;
        std::cout << "-------------------------------------------------------------------------" << std::endl;
        for (int k = 0; k < stack_num; k++) {
            PrintTitle(k);
            PrintDataHeader();
            for (int i = 1; i < max_thread; i++) {
                R_num = max_thread - i;
                for (int j = 1; j < R_num; j++) {
                    PrintData(C_data_matrix[k][i][j]);
                }
            }
        }
    }


    free(my_block_stack);
    free(my_non_block_stack);
    return 0;
}