//
// Created by dmitrii on 14.12.2019.
//

#ifndef LAB3_TESTRUNNER_H
#define LAB3_TESTRUNNER_H

#include "../interfaces/Stack.h"
#include "Test.h"
#include <utility>
#include <vector>
#include <map>

class TestRunner {
    std::vector<Test> m_tests;

    static void *writeToStack(void *arg);
    static void *readFromStack(void *arg);

    static void runWorkerTest(Stack* s, void*(*workerFunc)(void*), const TestParams& test_params);
    static int runWritersTest(Stack *s, const FullTestParams &test_params);
    static int runReadersTest(Stack *s, const FullTestParams &test_params);
    static int runFullTest(Stack *s, const FullTestParams &test_params);
public:
    explicit TestRunner(const FullTestParams& test_params);
    void runTests();

    static std::vector<pthread_t> runWorkers(const TestParams &test_params, std::vector<std::vector<int>> &worker_vecs, void *(*workerFunc)(void *),
               Stack *s);
};

struct ThreadArgs
{
    ThreadArgs(Stack* _s, std::vector<int>& test_vec, const int& action_count) : s(_s), m_test_vec(test_vec), m_action_count(action_count) {}
    Stack* s;
    std::vector<int>& m_test_vec;
    int m_action_count;
};


#endif //LAB3_TESTRUNNER_H
