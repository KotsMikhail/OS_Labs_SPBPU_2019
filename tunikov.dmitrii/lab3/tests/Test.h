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
};

class Test {
public:
    Test(const std::string& name, const TestParams& test_params, int (*_testFunc)(Stack* s, const TestParams& testParams));
    std::string m_name;
    TestParams m_test_params;
    int (*testFunc)(Stack* s, const TestParams& testParams);
    void runTest(Stack *s);
};


#endif //LAB3_TEST_H
