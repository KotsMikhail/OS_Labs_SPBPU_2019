//
// Created by dmitrii on 14.12.2019.
//

#include "TestRunner.h"
#include "../blocking_stack/BlockingStack.h"
#include "../exceptions/TimeoutException.h"
#include <map>

int TestRunner::runWritersTest(Stack* s, const TestParams& test_params) {
    int writers_count = test_params.workers_count;
    int write_actions_count = test_params.worker_actions_count;

    //fill test map
    std::map<int, int> test_map{};
    for (int j = 0; j < write_actions_count; j++)
        test_map.insert(std::pair<int, int>(j, writers_count));

    auto thread_args = new WriterThreadArgs(s, write_actions_count);

    runWorkers(s, writers_count, writeToStack, thread_args);

    //increment test_map by stack real data
    while (!s->empty())
    {
        int val = 0;
        s->pop(val);

        test_map[val]--;
    }

    checkAllElemIsZero(test_map);

    return 0;
}

int TestRunner::runReadersTest(Stack * s, const struct TestParams & test_params) {
    int readers_count = test_params.workers_count;
    int read_actions_count = test_params.worker_actions_count;

    //fill test map
    std::map<int, int> test_map{};
    for (int j = 0; j < read_actions_count; j++)
    {
        for (int i = 0; i < readers_count; i++)
        {
            s->push(j);
        }
        test_map.insert(std::pair<int, int>(j, readers_count));
    }

    auto thread_args = new ReaderThreadArgs(s, test_map);

    runWorkers(s, readers_count, &readFromStack, thread_args);

    checkAllElemIsZero(test_map);

    return 0;
}

void TestRunner::runWorkers(Stack* s, const int workers_count, void*(*workerFunc)(void*), void* worker_func_args)
{
    //init default arrts
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    std::vector<pthread_t> threadVec{};
    for (int i = 0; i < workers_count; i++){
        pthread_t tid;

        pthread_create(&tid, &attr, workerFunc, worker_func_args);
        threadVec.push_back(tid);
    }

    for (int i = 0; i < workers_count; i++)
        pthread_join(threadVec[i],nullptr);
}

void* TestRunner::writeToStack(void *arg) {
    auto* args = (WriterThreadArgs*)arg;

    Stack* s = args->s;
    int n = args->n;

    for (int i = 0; i < n; i++)
    {
        try {
            s->push(i);
        }
        catch(const TimeoutException& e)
        {
            pthread_yield();
            throw e;
        }
    }
    return nullptr;
}

void *TestRunner::readFromStack(void *arg) {
    auto* args = (ReaderThreadArgs*)arg;

    Stack* s = args->s;
    std::map<int, int>& test_map = args->m_test_map;

    while (!s->empty())
    {
        try {
            int val = 0;
            s->pop(val);
            test_map[val]--;
        }
        catch(const TimeoutException& e)
        {
            pthread_yield();
            throw e;
        }
    }
    return nullptr;
}

void TestRunner::runTests() {
    for (auto test : m_tests)
    {
        try{
            BlockingStack s = BlockingStack::make();
            test.runTest(&s);

            //TODO non-block stack
        }
        catch (const std::runtime_error& e)
        {
            std::cout << "ERROR while executing tests: " << e.what() << std::endl;
        }
    }
}

TestRunner::TestRunner(const TestRunnerParams &test_params) {
    m_tests.emplace_back(Test("Writers test", test_params.writer_params, &runWritersTest));
    m_tests.emplace_back(Test("Readers test", test_params.reader_params, &runReadersTest));
    m_tests.emplace_back(Test("Writers/Readers test", test_params.writer_params, &runFullTest));
}

int TestRunner::runFullTest(Stack *s, const TestParams &test_params) {
    return 0;
}

void TestRunner::checkAllElemIsZero(const std::map<int, int> &map) {
    for (auto elem : map)
    {
        if (elem.second != 0)
            throw std::runtime_error("wrong test map value for: " + std::to_string(elem.first));
    }
}
