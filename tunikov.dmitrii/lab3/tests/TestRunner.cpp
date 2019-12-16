//
// Created by dmitrii on 14.12.2019.
//

#include <zconf.h>
#include <thread>
#include "TestRunner.h"
#include "../lock_stack/LockStack.h"
#include <stdexcept>
#include "../lock_free_stack/LockFreeStack.h"
#include "../utils/utils.h"

int TestRunner::runWritersTest(Stack* s, const FullTestParams& test_params) {
    runWorkerTest(s, &writeToStack, test_params.writer_params);
    return 0;
}

int TestRunner::runReadersTest(Stack * s, const struct FullTestParams & test_params)
{
    for (int i = 0; i < test_params.reader_params.workers_count; i++)
        for (int j = 0; j < test_params.reader_params.worker_actions_count; j++)
        {
            s->push(j);
        }

    runWorkerTest(s, &readFromStack, test_params.reader_params);
    return 0;
}

int TestRunner::runFullTest(Stack *s, const FullTestParams &test_params) {
    int reader_action_count = test_params.reader_params.worker_actions_count;
    int writer_action_count = test_params.writer_params.worker_actions_count;

    int threads_max_count = (int)std::thread::hardware_concurrency();
    for (int cur_readers_count = test_params.reader_params.workers_count - 1; cur_readers_count < test_params.reader_params.workers_count; cur_readers_count++)
        for (int cur_writers_count = test_params.writer_params.workers_count - 1; cur_writers_count < test_params.writer_params.workers_count; cur_writers_count++)
        {
            if (cur_readers_count + cur_writers_count > threads_max_count)
                continue;

            std::vector<std::vector<int>> writers_vecs;
            std::vector<std::vector<int>> readers_vecs;

            //run writers
            auto writers_threads = std::move(runWorkers(TestParams(cur_writers_count, writer_action_count), writers_vecs, writeToStack, s));
            //run readers
            auto readers_threads = std::move(runWorkers(TestParams(cur_readers_count, reader_action_count), readers_vecs, readFromStack, s));

            //join all threads
            utils::joinThreads(writers_threads);
            utils::joinThreads(readers_threads);

            /*
            std::cout << "writers count: " << cur_writers_count << ", readers count: " << cur_readers_count << std::endl;
            std::cout << "writer vecs: " << std::endl;
            utils::printVectors(writers_vecs);
            std::cout << "reader vecs: " << std::endl;
            utils::printVectors(readers_vecs);*/

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

std::vector<pthread_t> TestRunner::runWorkers(const TestParams& test_params, std::vector<std::vector<int>>& worker_vecs, void*(*workerFunc)(void*), Stack* s)
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

void* TestRunner::writeToStack(void *arg) {
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

void *TestRunner::readFromStack(void *arg) {
    auto* args = (ThreadArgs*)arg;

    Stack* s = args->s;
    std::vector<int>& test_vec = args->m_test_vec;

    while (!s->empty())
    {
        try {
            auto val = s->pop();
            if (val != std::shared_ptr<int>())
                test_vec.emplace_back(*val);
        }
        catch(const std::runtime_error& e)
        {
            std::cout << "catch read exception" << std::endl;
            pthread_yield();
            throw e;
        }
    }
    return nullptr;
}

void TestRunner::runTests() {
    static std::vector<StackType> available_stack_types{LOCK, LOCK_FREE};
    for (auto stack_type : available_stack_types)
    {
        Stack* stack = nullptr;
        switch (stack_type)
        {
            case LOCK_FREE:
                std::cout << "Running tests for LOCK_FREE_STACK: " << std::endl;
                stack = new LockFreeStack();
                break;
            case LOCK:
                std::cout << "Running tests for LOCK_STACK: " << std::endl;
                stack = LockStack::make();
                break;
            default:
                throw std::runtime_error("ERROR: unknown stack type");
        }

        for (auto test : m_tests)
        {
            try{
                test.runTest(stack);
            }
            catch (const std::runtime_error& e)
            {
                std::cout << "ERROR while executing tests: " << e.what() << std::endl;
            }
        }
        std::cout << "--------------------------------------------------------" << std::endl;
    }
}

TestRunner::TestRunner(const FullTestParams &test_params) {
    m_tests.emplace_back(Test("Writers test", test_params, &runWritersTest));
    m_tests.emplace_back(Test("Readers test", test_params, &runReadersTest));
    m_tests.emplace_back(Test("Writers/Readers test", test_params, &runFullTest));
}