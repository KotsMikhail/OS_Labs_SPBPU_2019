//
// Created by dmitrii on 16.12.2019.
//


#include "Stack.h"
#include "../tests/TestRunner.h"

void *Stack::readFromStack(void *arg) {
    auto* args = (ThreadArgs*)arg;

    Stack* s = args->s;
    int read_count = args->m_action_count;
    std::vector<int>& test_vec = args->m_test_vec;

    while (test_vec.size() != read_count) {
        if (!s->empty()) {
            try {
                auto val = s->pop();
                if (val != std::shared_ptr<int>())
                    test_vec.emplace_back(*val);
            }
            catch (const std::runtime_error &e) {
                pthread_yield();
                throw e;
            }
        }
    }

    return nullptr;
}

void *Stack::writeToStack(void *arg) {
    auto* args = (ThreadArgs*)arg;

    Stack* s = args->s;
    int n = args->m_action_count;
    std::vector<int>& test_vec = args->m_test_vec;

    for (int i = 0; i < n; i++)
    {
        try {
            s->push(i);
            test_vec.emplace_back(i);
        }
        catch(const std::runtime_error& e)
        {
            pthread_yield();
        }
    }
    return nullptr;
}
