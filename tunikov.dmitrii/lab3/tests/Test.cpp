//
// Created by dmitrii on 14.12.2019.
//

#include "Test.h"

Test::Test(const std::string &name, const TestParams &test_params, int (*_testFunc)(Stack *, const TestParams &)) {
    m_name = name;
    testFunc = _testFunc;
    m_test_params = test_params;
}

void Test::runTest(Stack* s) {
    try
    {
        int res = testFunc(s, m_test_params);

        if (res == 0)
            std::cout << m_name << ": " << "OK" << std::endl;
        else
            std::cout << m_name << ": " << "FAIL" << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cout << m_name << ": " << "FAIL, because " << e.what() << std::endl;
    }
}
