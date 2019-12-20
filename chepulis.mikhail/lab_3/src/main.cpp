#include <iostream>

#include "Tester.h"

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


bool CheckThreadCount(testParam& param) {
    int nproc_lim = GetNpocLim();
    if (nproc_lim < param.max_thread) {
        param.max_thread = nproc_lim;
    }
    if (param.W_num <= 0) {
        std::cout << "ERROR: W_num <= 0" << std::endl;
        return false;
    }
    if (param.R_num <= 0) {
        std::cout << "ERROR: R_num <= 0" << std::endl;
        return false;
    }
    if (param.W_num + param.R_num > nproc_lim) {
        std::cout << "ERROR: (W_num  + R_num) > thread limit" << std::endl;
        return false;
    }
    return true;

}

bool ParsArgs(int argc, char** agrv, testParam& result ) {
    if (argc < 6) {
        std::cout << "ERROR: Wrong arguments" << std::endl;
        return false;
    }
    try {
        result.N_big = std::stoi(agrv[1]);
        result.R_num = std::stoi(agrv[2]);
        result.W_num = std::stoi(agrv[3]);
        result.N = std::stoi(agrv[4]);
        result.max_thread = std::stoi(agrv[5]);

        if (result.N <= 0) {
            std::cout << "ERROR: N <= 0" << std::endl;
            return false;
        }
        if (result.N_big <= 0) {
            std::cout << "ERROR: N_big <= 0" << std::endl;
            return false;
        }
        return CheckThreadCount(result);
    } catch (std::exception e) {
        std::cout << "ERROR: Wrong arguments: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char** argv) {

    std::cout << std::boolalpha;
    testParam params;
    if (!ParsArgs(argc, argv, params)) {
        return -1;
    }

    int repeat = 10;
    Tester tester;

    tester.DoTest(params.W_num, params.R_num, params.N_big, repeat);
    tester.DoIterTest(params.N, params.max_thread, repeat);

    return 0;
}