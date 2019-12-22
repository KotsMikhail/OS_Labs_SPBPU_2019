#include <pthread.h>
#include <iostream>
#include <atomic>

#include "test_utils.h"


void prepareRecordsPerThreadArray (int numThreads, int numRecords, std::vector<int> &recordsPerThread)
{
    if (numRecords > numThreads) {
        int n = numRecords / numThreads;
        int m = numRecords - n * numThreads;

        int i = 0;
        for (; i < m; i++) {
            recordsPerThread[i] = n + 1;
        }

        for (; i < numThreads; i++) {
            recordsPerThread[i] = n;
        }
    } else {
        int i = 0;
        for (; i < numRecords; i++) {
            recordsPerThread[i] = 1;
        }

        for (; i < numThreads; i++) {
            recordsPerThread[i] = 0;
        }
    }
}


static void * _writeThreadFunc (void * args)
{
    TEST_INFO *testInfo = (TEST_INFO *)args;
    SET<int> *set = testInfo->set;

    for (int val : testInfo->data) {
        set->Add(val);
    }

    pthread_exit(nullptr);
}


static void * _readThreadFunc (void * args)
{
    TEST_INFO *testInfo = (TEST_INFO *)args;
    SET<int> *set = testInfo->set;

    int dataSize = testInfo->data.size();
    for (int i = 0; i < dataSize; i++) {
        set->Remove(testInfo->data[i]);
    }

    pthread_exit(nullptr);
}


static void * _readWithMissCountThreadFunc (void * args)
{
    const int missCountLimit = 1;
    TEST_INFO *testInfo = (TEST_INFO *)args;
    SET<int> *set = testInfo->set;

    for (int val : testInfo->data) {
        int missCount = 0;
        while (!set->Remove(val)) {
            if (!*testInfo->isWritersRunning && missCount++ >= missCountLimit) {
                pthread_exit(nullptr);
            }

            pthread_yield();
        }

        int checkVal;
        do {
            checkVal = testInfo->checkArr[val];
        } while (!__sync_bool_compare_and_swap(testInfo->checkArr + val, checkVal, checkVal + 1));
    }

    pthread_exit(nullptr);
}


THREAD_FUNC_SIGNATURE getThreadFunc (const TESTING_THREAD_FUNC_ID &threadFuncId)
{
    switch (threadFuncId) {
        case TESTING_THREAD_FUNC_ID::READ:
            return _readThreadFunc;
        case TESTING_THREAD_FUNC_ID::WRITE:
            return _writeThreadFunc;
        case TESTING_THREAD_FUNC_ID::READ_WITH_MISS_COUNT:
            return _readWithMissCountThreadFunc;
        default:
            return nullptr;
    }
}


int createThreads (std::vector<pthread_t> &threads, THREAD_FUNC_SIGNATURE threadsFunc, std::vector<TEST_INFO *> testInfos)
{
    int numThreads = threads.size();
    for (int i = 0; i < numThreads; i++) {
        int res = pthread_create(&threads[i], nullptr, threadsFunc, testInfos[i]);
        if (res != 0) {
            std::cout << "Couldn't create thread with index " << i << ", error code - " << res << std::endl;
            std::cout << "Were created " << i << "/" << numThreads << " threads" << std::endl;
            return i;
        }
    }

    return numThreads;
}


void delTestInfos (std::vector<TEST_INFO *> testInfos)
{
    for (TEST_INFO *t : testInfos) {
        delete t;
    }
}


void joinThreads (std::vector<pthread_t> &threads, int numThreadsToJoin)
{
    for (int i = 0; i < numThreadsToJoin; i++) {
        pthread_join(threads[i], nullptr);
    }
}


void getStartTime (struct timespec &startTime)
{
    clock_gettime(CLOCK_REALTIME, &startTime);
}


void getEndTime (const struct timespec &startTime, double *time)
{
    struct timespec curTime;
    clock_gettime(CLOCK_REALTIME, &curTime);
    __time_t secs = curTime.tv_sec - startTime.tv_sec;
    __syscall_slong_t nsecs = curTime.tv_nsec - startTime.tv_nsec;
    *time = secs + (double) nsecs / 1e9;
}
