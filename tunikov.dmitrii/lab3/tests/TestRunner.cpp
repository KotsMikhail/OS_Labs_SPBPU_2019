//
// Created by dmitrii on 14.12.2019.
//

#include <zconf.h>
#include "TestRunner.h"
#include "../lock_stack/LockStack.h"
#include <stdexcept>
#include "../lock_free_stack/LockFreeStack.h"
#include "../utils/utils.h"
#include <cstdio>
#include <ctime>

unsigned TestRunner::time_tests_count = 100;

int TestRunner::runWritersTest(Stack* s, const FullTestParams& test_params) {
    runWorkerTest(s, &Stack::writeToStack, test_params.writer_params);
    return 0;
}

int TestRunner::runReadersTest(Stack * s, const struct FullTestParams & test_params)
{
    for (int i = 0; i < test_params.reader_params.workers_count; i++)
        for (int j = 0; j < test_params.reader_params.worker_actions_count; j++)
        {
            s->push(j);
        }

    runWorkerTest(s, &Stack::readFromStack, test_params.reader_params);
    return 0;
}

int TestRunner::runFullTest(Stack *s, const FullTestParams &test_params) {
    int writer_action_count = test_params.writer_params.worker_actions_count;

    int threads_max_count = utils::getMaxThreadsCount();
    for (int cur_readers_count = 1; cur_readers_count < 5; cur_readers_count++)
        for (int cur_writers_count = 1; cur_writers_count < 5; cur_writers_count++)
        {
            if (cur_readers_count + cur_writers_count > threads_max_count)
                continue;

            std::vector<std::vector<int>> writers_vecs;
            std::vector<std::vector<int>> readers_vecs;

            int full_write_count = cur_writers_count * writer_action_count;
            int one_reader_action_count = full_write_count / cur_readers_count;
            int balance_for_last_reader = full_write_count - one_reader_action_count * cur_readers_count;

            auto readers_threads = std::move(runWorkers(TestParams(cur_readers_count, one_reader_action_count), readers_vecs, Stack::readFromStack, s, balance_for_last_reader));
            auto writers_threads = std::move(runWorkers(TestParams(cur_writers_count, writer_action_count), writers_vecs, Stack::writeToStack, s));
            utils::joinThreads(readers_threads);
            utils::joinThreads(writers_threads);

            for (auto& writer_vec : writers_vecs)
            {
                for (auto& writer_vec_elem : writer_vec)
                {
                    bool was_found = utils::containsAndErase(readers_vecs, writer_vec_elem);

                    if (!was_found)
                    {
                        std::string error_msg = "for writers count: " + std::to_string(cur_writers_count) + ", readers count: " + std::to_string(cur_readers_count)
                                                + ", error for elem: " + std::to_string(writer_vec_elem);
                        throw std::runtime_error(error_msg);
                    }
                }
            }
        }

    return 0;
}

std::vector<pthread_t> TestRunner::runWorkers(const TestParams& test_params, std::vector<std::vector<int>>& worker_vecs, void*(*workerFunc)(void*), Stack* s,
                                              int balance_for_last_worker)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    //run worker
    std::vector<pthread_t> threadVec{};
    worker_vecs.clear();
    for (int i = 0; i < test_params.workers_count; i++)
    {
        std::vector<int> v = std::vector<int>();
        worker_vecs.push_back(v);
    }

    for (int i = 0; i < test_params.workers_count; i++)
    {
        auto* thread_args = new ThreadArgs(s, worker_vecs[i], test_params.worker_actions_count);

        if (i == test_params.workers_count - 1)
            thread_args->m_action_count = test_params.worker_actions_count + balance_for_last_worker;

        pthread_t tid;

        pthread_create(&tid, &attr, workerFunc, thread_args);
        threadVec.push_back(tid);
    }

    return threadVec;
}

void TestRunner::runWorkerTest(Stack* s, void*(*workerFunc)(void*), const TestParams& test_params)
{
    std::vector<std::vector<int>> worker_vecs;

    auto threads = std::move(runWorkers(test_params, worker_vecs, workerFunc, s));
    utils::joinThreads(threads);

    //test workers vectors
    for (int i = 0; i < test_params.workers_count; i++)
        for (int j = 0; j < test_params.worker_actions_count; j++)
        {
            bool was_found = utils::containsAndErase(worker_vecs, j);

            if (!was_found)
            {
                throw std::runtime_error(std::string("wasn't found elem: ") + std::to_string(j));
            }
        }
}

void TestRunner::runTests() {
    static std::vector<StackType> available_stack_types{LOCK, LOCK_FREE};
    runFuncTests(available_stack_types);
    std::cout << "-------------------------------------" << std::endl;
    runTimeTests(available_stack_types);
}

TestRunner::TestRunner(const FullTestParams &test_params) {
    m_tests.emplace_back(Test("Writers test", test_params, &runWritersTest));
    m_tests.emplace_back(Test("Readers test", test_params, &runReadersTest));
    m_tests.emplace_back(Test("Writers/Readers test", test_params, &runFullTest));
}

void TestRunner::runTimeTests(const std::vector<StackType>& available_stack_types)
{
    std::cout << "Running time tests for writers_count[1..5] and readers_count[1..5] (all permutations of readers and writers count): " << std::endl;

    int space_count = 23;
    for (int i = 0; i < space_count; i++)
        std::cout << " ";
    std::cout << "LOCK_STACK\t" << "LOCK_FREE_STACK\t" << std::endl;

    for (auto& test : m_tests)
    {
        std::cout << test.m_name;
        for (int i = test.m_name.size(); i < space_count; i++)
            std::cout << " ";

        for (const auto& stack_type : available_stack_types)
        {
            std::vector<unsigned long> tests_times;
            for (unsigned i = 0; i < time_tests_count; i++)
            {
                unsigned long start_time = clock();
                Stack* s = nullptr;
                try {
                    if (stack_type == LOCK)
                        s = LockStack::make();
                    else
                        s = new LockFreeStack();
                    test.runTest(s, false);
                    delete s;
                }
                catch (const std::runtime_error &e) {
                    std::cout << "ERROR while executing tests: " << e.what() << std::endl;
                    delete s;
                }

                unsigned long end_time = clock();
                tests_times.push_back((float)(end_time - start_time) * 1000 / CLOCKS_PER_SEC);
            }
            float aver_time = utils::getAverage(tests_times);
            std::cout << aver_time << "ms\t\t";
        }
        std::cout << "\n";
    }
}

void TestRunner::runFuncTests(const std::vector<StackType>& available_stack_types) {
    std::cout << "Running functional tests: " << std::endl;
    for (auto stack_type : available_stack_types) {
        if (stack_type == LOCK_FREE)
            std::cout << "LOCK_FREE_STACK: " << std::endl;
        else
            std::cout << "LOCK_STACK: " << std::endl;

        for (auto test : m_tests) {
            Stack *stack = nullptr;
            if (LOCK_FREE == stack_type)
                stack = new LockFreeStack();
            else
                stack = LockStack::make();
            try {
                test.runTest(stack);
            }
            catch (const std::runtime_error &e) {
                std::cout << "ERROR while executing tests: " << e.what() << std::endl;
            }

            delete stack;
        }
    }
}
