//
// Created by dmitrii on 14.12.2019.
//

#ifndef LAB3_TESTRUNNER_H
#define LAB3_TESTRUNNER_H

#include "../interfaces/Stack.h"
#include "Test.h"
#include <vector>
#include <map>

struct TestRunnerParams{
    TestParams writer_params;
    TestParams reader_params;
};

class TestRunner {
    std::vector<Test> m_tests;

    static void *writeToStack(void *arg);
    static void *readFromStack(void *arg);

    static void runWorkers(Stack* s, const int workers_count, void*(*workerFunc)(void*), void* worker_func_args);

    static void checkAllElemIsZero(const std::map<int, int>& map);
    static int runWritersTest(Stack *s, const TestParams &test_params);
    static int runReadersTest(Stack *s, const TestParams &test_params);
    static int runFullTest(Stack *s, const TestParams &test_params);
public:
    explicit TestRunner(const TestRunnerParams& test_params);
    void runTests();
};

struct ThreadArgs
{
    ThreadArgs(Stack* _s) : s(_s) {}
    Stack* s;
};

struct WriterThreadArgs : public ThreadArgs {
    WriterThreadArgs(Stack* _s, int _n) : ThreadArgs(_s), n(_n) {}
    int n;
};

struct ReaderThreadArgs : public ThreadArgs
{
    ReaderThreadArgs(Stack* _s, std::map<int, int>& test_map) : ThreadArgs(_s), m_test_map(test_map){}
    std::map<int, int>& m_test_map;
};


#endif //LAB3_TESTRUNNER_H
