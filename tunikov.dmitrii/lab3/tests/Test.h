//
// Created by dmitrii on 14.12.2019.
//

#ifndef LAB3_TEST_H
#define LAB3_TEST_H

#include <iostream>
#include "../interfaces/Stack.h"

struct TestParams
{
    int workers_count;
    int worker_actions_count;
    TestParams(const int& _workers_count, const int& _worker_actions_count) : workers_count(_workers_count), worker_actions_count(_worker_actions_count){}
    TestParams()= default;
};

struct FullTestParams
{
    TestParams writer_params;
    TestParams reader_params;
};

class Test {
public:
    Test(const std::string& name, const FullTestParams& test_params, int (*_testFunc)(Stack* s, const FullTestParams& testParams));
    std::string m_name;
    FullTestParams m_test_params;
    int (*testFunc)(Stack* s, const FullTestParams& testParams);
    void runTest(Stack *s, bool log_result = true);
};


#endif //LAB3_TEST_H
