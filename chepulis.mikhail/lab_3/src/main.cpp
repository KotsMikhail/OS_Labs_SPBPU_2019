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

struct testParam{
    int N_big;                  // размер массива для единоразового теста
    int R_num;                  // кол-во писателей для единоразового теста
    int W_num;                  // кол-во читателей для единоразового теста
    int N;                      // размер массива
    int max_thread;             // макс кол-во потоков для перебора
};


bool CheckThreadCount(testParam& param){
    int nproc_lim = GetNpocLim();
    if (nproc_lim < param.max_thread) {
        param.max_thread = nproc_lim;
    }
    if(param.W_num <= 0){
        std::cout << "ERROR: W_num <= 0" << std::endl;
        return false;
    }
    if(param.R_num <= 0){
        std::cout << "ERROR: R_num <= 0" << std::endl;
        return false;
    }
    if(param.W_num + param.R_num > nproc_lim){
        std::cout << "ERROR: (W_num  + R_num) > thread limit" << std::endl;
        return false;
    }
    return true;

}

bool ParsArgs(int argc, char** agrv, testParam& result ){
     if (argc < 6)
     {
         std::cout << "ERROR: Wrong arguments" << std::endl;
         return false;
     }
     try {
         result.N_big = std::stoi(agrv[1]);
         result.R_num = std::stoi(agrv[2]);
         result.W_num = std::stoi(agrv[3]);
         result.N = std::stoi(agrv[4]);
         result.max_thread = std::stoi(agrv[5]);

         if(result.N <= 0){
             std::cout << "ERROR: N <= 0" << std::endl;
             return false;
         }
         if(result.N_big <= 0){
             std::cout << "ERROR: N_big <= 0" << std::endl;
             return false;
         }
         return CheckThreadCount(result);
     }catch(std::exception e)
     {
         std::cout << "ERROR: Wrong arguments: " << e.what() << std::endl;
         return false;
     }


}

int main(int argc, char** argv) {
    bool test_res;

    IStack *my_block_stack = new BlockedStack;
    IStack *my_non_block_stack = new NonBlockedStack;

    Timer timer;
    ReaderTester R_tester;
    WriterTester W_tester;
    CommonTester C_tester;
    std::cout << std::boolalpha;

    testParam params;
    if (!ParsArgs(argc, argv, params)) {
        return -1;
    }


    int N, R_num, W_num, max_thread, stack_num, repeat;
    max_thread = params.max_thread;
    N = params.N;
    stack_num = 2;
    repeat = 10;


    R_num = params.R_num;
    W_num = params.W_num;
    int local_N;
    local_N = params.N_big;
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


    IStack *stacks[stack_num];
    stacks[0] = my_block_stack;
    stacks[1] = my_non_block_stack;

    std::cout << std::endl << std::endl;

    TestData R_data_matrix[stack_num][max_thread];
    TestData W_data_matrix[stack_num][max_thread];
    TestData C_data_matrix[stack_num][max_thread][max_thread];
    bool test_result[stack_num];


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


    delete my_block_stack;
    delete my_non_block_stack;
    return 0;
}