#include <new>

#include "general_test.h"


GENERAL_TEST_RUNNER::GENERAL_TEST_RUNNER ()
{
    this->id = AVAILABLE_TEST_RUNNER_ID::GENERAL;
}


bool GENERAL_TEST_RUNNER::Check (int *checkArr, int numOfRecords)
{
    bool res = true;
    for (int i = 0; i < numOfRecords; i++) {
        if (checkArr[i] != 1) {
            std::cout << "Failed checking general test. Index " << i << " has value " << checkArr[i] << std::endl;
            res = false;
        }
    }

    return res;
}


bool GENERAL_TEST_RUNNER::Run (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck)
{
    return RunTest(set, dataSetCreator, needCheck, nullptr);
}


bool GENERAL_TEST_RUNNER::RunWithTimeMeasure (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time)
{
    return RunTest(set, dataSetCreator, needCheck, time);
}


bool GENERAL_TEST_RUNNER::RunTest(SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time)
{
    CONFIG *config = CONFIG::GetInstance();
    int numWriters = config->GetValue(CONFIG::KEYS::GENERAL_WRITERS);
    int numReaders = config->GetValue(CONFIG::KEYS::GENERAL_READERS);
    int numRecords = config->GetValue(CONFIG::KEYS::GENERAL_N);

    std::vector<int> recordsPerWriter(numWriters);
    std::vector<int> recordsPerReader(numReaders);

    prepareRecordsPerThreadArray(numReaders, numRecords, recordsPerReader);
    prepareRecordsPerThreadArray(numWriters, numRecords, recordsPerWriter);

    std::vector<TEST_INFO *> readTestInfos(numReaders);
    std::vector<TEST_INFO *> writeTestInfos(numWriters);

    std::vector<pthread_t> readThreads(numReaders);
    std::vector<pthread_t> writeThreads(numWriters);

    std::vector<std::vector<int>> readDataSets(numReaders);
    std::vector<std::vector<int>> writeDataSets(numWriters);

    int *checkArr = new (std::nothrow) int[numRecords]();
    bool isWritersRunning = true;

    struct timespec startTime;

    if (!dataSetCreator->Create(set, numReaders, recordsPerReader, readDataSets, readTestInfos, false)) {
        return false;
    }

    for (TEST_INFO *testInfo : readTestInfos) {
        testInfo->checkArr = checkArr;
        testInfo->isWritersRunning = &isWritersRunning;
    }

    if (!dataSetCreator->Create(set, numWriters, recordsPerWriter, writeDataSets, writeTestInfos, false)) {
        delTestInfos(readTestInfos);
        delete [] checkArr;
        return false;
    }

    if (time) {
        getStartTime(startTime);
    }

    int writeNThr = createThreads(writeThreads, getThreadFunc(TESTING_THREAD_FUNC_ID::WRITE), writeTestInfos);
    int readNThr = createThreads(readThreads, getThreadFunc(TESTING_THREAD_FUNC_ID::READ_WITH_MISS_COUNT), readTestInfos);

    joinThreads(writeThreads, writeNThr);
    isWritersRunning = false;
    joinThreads(readThreads, readNThr);

    if (time) {
        getEndTime(startTime, time);
    }

    delTestInfos(writeTestInfos);
    delTestInfos(readTestInfos);

    if (readNThr != numReaders || writeNThr != numWriters) {
        delete [] checkArr;
        return false;
    }

    if (needCheck) {
        if (!Check(checkArr, numRecords)) {
            std::cout << "Fail." << std::endl;
        }
    }

    delete [] checkArr;
    return true;
}
