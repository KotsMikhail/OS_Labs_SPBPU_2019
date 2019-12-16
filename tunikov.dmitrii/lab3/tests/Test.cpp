//
// Created by dmitrii on 14.12.2019.
//

#include "Test.h"

Test::Test(const std::string &name, const FullTestParams &test_params, int (*_testFunc)(Stack *, const FullTestParams &)) {
    m_name = name;
    testFunc = _testFunc;
    m_test_params = test_params;
}

void Test::runTest(Stack* s, bool log_result) {
    try
    {
        int res = testFunc(s, m_test_params);

        if (log_result)
        {
            if (res == 0)
                std::cout << m_name << ": " << "OK" << std::endl;
            else
                std::cout << m_name << ": " << "FAIL" << std::endl;
        }
    }
    catch (const std::runtime_error& e)
    {
        if (log_result)
            std::cout << m_name << ": " << "FAIL, because " << e.what() << std::endl;
        else
            throw e;
    }
}
