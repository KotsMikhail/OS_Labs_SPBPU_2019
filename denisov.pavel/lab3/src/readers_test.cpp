#include <iostream>

#include "readers_test.h"


READERS_TEST_RUNNER::READERS_TEST_RUNNER ()
{
    this->id = AVAILABLE_TEST_RUNNER_ID::READERS;
}


bool READERS_TEST_RUNNER::Check (SET<int> *set, std::vector<std::vector<int>> dataSets)
{
    if (set->IsEmpty())
        return true;

    for (std::vector<int> dataSet : dataSets) {
        for (int val : dataSet) {
            if (set->Contains(val)) {
                std::cout << "Failed readers check. Set contains value " << val << std::endl;
            }
        }
    }

    return true;
}


bool READERS_TEST_RUNNER::Run (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck)
{
    return RunTest(set, dataSetCreator, needCheck, nullptr);
}


bool READERS_TEST_RUNNER::RunWithTimeMeasure (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time)
{
    return RunTest(set, dataSetCreator, needCheck, time);
}


bool READERS_TEST_RUNNER::RunTest(SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time)
{
    CONFIG *config = CONFIG::GetInstance();
    int numThreads = config->GetValue(CONFIG::KEYS::READERS_NUM_THREADS);
    int numRecords = config->GetValue(CONFIG::KEYS::READERS_NUM_RECORDS);

    std::vector<int> numOfRecordsPerThread(numThreads);
    prepareRecordsPerThreadArray(numThreads, numRecords, numOfRecordsPerThread);

    std::vector<TEST_INFO *> testInfos(numThreads, nullptr);
    std::vector<pthread_t> threads(numThreads);
    std::vector<std::vector<int>> dataSets(numThreads);

    struct timespec startTime;

    if (!dataSetCreator->Create(set, numThreads, numOfRecordsPerThread, dataSets, testInfos, true)) {
        return false;
    }

    if (time) {
        getStartTime(startTime);
    }

    int nThr = createThreads(threads, getThreadFunc(TESTING_THREAD_FUNC_ID::READ), testInfos);
    joinThreads(threads, nThr);

    if (time) {
        getEndTime(startTime, time);
    }

    delTestInfos(testInfos);
    if (nThr != numThreads) {
        return false;
    }

    // check
    if (needCheck) {
        if (!Check(set, dataSets)) {
            std::cout << "Fail." << std::endl;
        }
    }

    return true;
}
