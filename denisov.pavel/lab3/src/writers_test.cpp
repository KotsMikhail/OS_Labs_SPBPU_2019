#include <iostream>

#include "writers_test.h"


WRITERS_TEST_RUNNER::WRITERS_TEST_RUNNER ()
{
    this->id = AVAILABLE_TEST_RUNNER_ID::WRITERS;
}


bool WRITERS_TEST_RUNNER::Check (SET<int> *set, std::vector<std::vector<int>> dataSets)
{
    bool res = true;
    for (std::vector<int> dataSet : dataSets) {
        for (int val : dataSet) {
            if (!set->Contains(val)) {
                std::cout << "Failed writers check. Set doesn't contain value " << val << std::endl;
                res = false;
            }
        }
    }

    return res;
}


bool WRITERS_TEST_RUNNER::Run (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck)
{
    return RunTest(set, dataSetCreator, needCheck, nullptr);
}


bool WRITERS_TEST_RUNNER::RunWithTimeMeasure (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time)
{
    return RunTest(set, dataSetCreator, needCheck, time);
}


bool WRITERS_TEST_RUNNER::RunTest(SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time)
{
    CONFIG *config = CONFIG::GetInstance();
    int numThreads = config->GetValue(CONFIG::KEYS::WRITERS_NUM_THREADS);
    int numRecords = config->GetValue(CONFIG::KEYS::WRITERS_NUM_RECORDS);

    std::vector<int> numOfRecordsPerThread(numThreads);
    prepareRecordsPerThreadArray(numThreads, numRecords, numOfRecordsPerThread);

    std::vector<TEST_INFO *> testInfos(numThreads, nullptr);
    std::vector<pthread_t> threads(numThreads);
    std::vector<std::vector<int>> dataSets(numThreads);

    struct timespec startTime;

    if (!dataSetCreator->Create(set, numThreads, numOfRecordsPerThread, dataSets, testInfos, false)) {
        return false;
    }

    if (time) {
        getStartTime(startTime);
    }

    int nThr = createThreads(threads, getThreadFunc(TESTING_THREAD_FUNC_ID::WRITE), testInfos);
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
            return false;
        }
    }

    return true;
}
