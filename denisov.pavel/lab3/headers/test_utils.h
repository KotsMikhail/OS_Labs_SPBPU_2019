#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include <vector>
#include <string>
#include <functional>
#include <iostream>

#include "test.h"
#include "set.h"
#include "config.h"


struct TEST_INFO
{
    SET<int>         *set;
    std::vector<int> &data;

    bool *isWritersRunning;  // for general test
    int  *checkArr;          // for general test

    TEST_INFO (SET<int> *s, std::vector<int> &d) : set(s), data(d), isWritersRunning(nullptr), checkArr(nullptr)
    {
    }
};


enum class AVAILABLE_DATA_SET_CREATOR_ID
{
    SIMPLE,
    FIXED,
    RANDOM
};


enum class AVAILABLE_TEST_RUNNER_ID
{
    WRITERS,
    READERS,
    GENERAL
};


class DATA_SET_CREATOR
{
public:
    DATA_SET_CREATOR () = default;
    virtual ~DATA_SET_CREATOR () = default;

    virtual bool Create (SET<int> *set, int numOfThreads, const std::vector<int> &numOfActionsPerThread,
                         std::vector<std::vector<int>> &dataSets, std::vector<TEST_INFO *> &testInfos,
                         bool addDataToSet = false) = 0;


    AVAILABLE_DATA_SET_CREATOR_ID id;
};


class TEST_RUNNER
{
public:
    TEST_RUNNER () {};
    virtual ~TEST_RUNNER () {};

    virtual bool Run                (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck)               = 0;
    virtual bool RunWithTimeMeasure (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time) = 0;

    AVAILABLE_TEST_RUNNER_ID id;
};


void prepareRecordsPerThreadArray (int numThreads, int numRecords, std::vector<int> &recordsPerThread);


enum class TESTING_THREAD_FUNC_ID
{
    WRITE,
    READ,
    READ_WITH_MISS_COUNT
};


typedef void * (*THREAD_FUNC_SIGNATURE) (void *);
THREAD_FUNC_SIGNATURE getThreadFunc (const TESTING_THREAD_FUNC_ID &threadFuncId);


int createThreads (std::vector<pthread_t> &threads, THREAD_FUNC_SIGNATURE threadsFunc, std::vector<TEST_INFO *> testInfos);


void delTestInfos (std::vector<TEST_INFO *> testInfos);
void joinThreads  (std::vector<pthread_t> &threads, int numThreadsToJoin);


void getStartTime (struct timespec &startTime);
void getEndTime   (const struct timespec &startTime, double *time);

#endif //__TEST_UTILS_H__
