#include <pthread.h>
#include <chrono>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

#include "tests.h"
#include "fine_list.h"
#include "optimistic_list.h"

using namespace std;
using namespace std::chrono;

class ThreadInfo
{
public:
    Set<int> *set;
    vector<int> *data, *array;
    int numberWriters, numberReaders, numberRecords, numberReadings, index;

    ThreadInfo()
    {};

    ThreadInfo(Set<int> *s, vector<int> *d, vector<int> *a, int nWriters, int nReaders, int nRecords, int nReadings, int i)
        : set(s), data(d), array(a), numberWriters(nWriters), numberReaders(nReaders), numberRecords(nRecords), numberReadings(nReadings), index(i)
    {};
};

vector<ThreadInfo> CreateThreadsInfo(int numberThreads, Set<int> *set, vector<int> *data, vector<int> *array, int numberWriters, int numberReaders, int numberRecords, int numberReadings)
{
    vector<ThreadInfo> threadsInfo(numberThreads);
    for (int i = 0; i < numberThreads; i++)
        threadsInfo[i] = ThreadInfo(set, data, array, numberWriters, numberReaders, numberRecords, numberReadings, i);
    
    return threadsInfo;
}

vector<pthread_t> CreateThreads(int numberThreads, void *(*startRoutine) (void *), vector<ThreadInfo> &threadsInfo)
{
    vector<pthread_t> threads(numberThreads);
    for (int i = 0; i < numberThreads; i++)
        pthread_create(&threads[i], nullptr, startRoutine, &threadsInfo[i]);
    
    return threads;
}

void JoinThreads(const vector<pthread_t> &threads)
{
    for (auto thread : threads)
        pthread_join(thread, nullptr);
}

void* Write(void *arg)
{
    ThreadInfo *threadInfo = (ThreadInfo*)arg;
    for (int i = 0; i < threadInfo->numberRecords; i++)
        threadInfo->set->add(threadInfo->data->at(threadInfo->index + i * threadInfo->numberWriters));
    
    return nullptr;
}

void* Read(void *arg)
{
    ThreadInfo *threadInfo = (ThreadInfo*)arg;
    for (int i = 0; i < threadInfo->numberReadings; i++)
    {
        while (!threadInfo->set->remove(threadInfo->data->at(threadInfo->index + i * threadInfo->numberReaders)))
            pthread_yield();
        
        threadInfo->array->at(threadInfo->data->at(threadInfo->index + i * threadInfo->numberReaders)) = 1;
    }
    
    return nullptr;
}

bool CheckTestWriters(Set<int> &set, vector<int> &array)
{
    for (auto i : array)
        if (!set.contains(i))
            return false;
    
    return true;
}

bool CheckTestReaders(Set<int> &set, vector<int> &array)
{
    for (auto i : array)
        if (i != 1)
            return false;

    return set.empty();
}

Set<int>* CreateSet(SetType setType)
{
    if (setType == SetType::FINE_LIST)
        return new FineList<int>();
    
    return new OptimisticList<int>();
}

vector<int> CreateDataFixed(int n)
{
    vector<int> data(n);
    iota(data.begin(), data.end(), 0);

    return data;
}

vector<int> CreateDataRandom(int n)
{
    vector<int> data = CreateDataFixed(n);
    random_shuffle(data.begin(), data.end());

    return data;
}

vector<int> CreateData(DataType dataType, int n)
{
    if (dataType == DataType::RANDOM)
        return CreateDataRandom(n);
    
    return CreateDataFixed(n);
}

string CreateNameTest(const string &type, SetType setType, DataType dataType)
{
    return (setType == SetType::FINE_LIST ? "FineList " : "OptimisticList ") + type + (dataType == DataType::RANDOM ? " random" : " fixed");
}

void PrintTestResult(const string &type, SetType setType, DataType dataType, bool isSuccess, double time)
{
    cout << CreateNameTest(type, setType, dataType) << (isSuccess ? ": SUCCESS " : ": FAIL ") << time << "." << endl;
}

void RunTestWriters(SetType setType, DataType dataType, int numberWriters, int numberRecords, int numberTimes)
{
    bool isSuccess = true;
    double timeTotal = 0;

    for (int i = 0; i < numberTimes; i++)
    {
        Set<int> *set = CreateSet(setType);
        vector<int> data = CreateData(dataType, numberWriters * numberRecords);
        vector<ThreadInfo> threadsInfo = CreateThreadsInfo(numberWriters, set, &data, nullptr, numberWriters, 0, numberRecords, 0);

        time_point<system_clock> start = system_clock::now();
        vector<pthread_t> threads = CreateThreads(numberWriters, Write, threadsInfo);
        JoinThreads(threads);
        time_point<system_clock> end = system_clock::now();

        isSuccess &= CheckTestWriters(*set, data);
        timeTotal += duration_cast<milliseconds>(end - start).count() / 1000.0;

        delete set;
    }

    PrintTestResult("test writers", setType, dataType, isSuccess, timeTotal / numberTimes);
}

void RunTestReaders(SetType setType, DataType dataType, int numberReaders, int numberReadings, int numberTimes)
{
    bool isSuccess = true;
    double timeTotal = 0;

    for (int i = 0; i < numberTimes; i++)
    {
        Set<int> *set = CreateSet(setType);
        vector<int> data = CreateData(dataType, numberReaders * numberReadings);
        vector<int> array = vector<int>(numberReaders * numberReadings, 0);
        vector<ThreadInfo> threadsInfo = CreateThreadsInfo(numberReaders, set, &data, &array, 0, numberReaders, 0, numberReadings);

        for (auto d : data)
            set->add(d);

        time_point<system_clock> start = system_clock::now();
        vector<pthread_t> threads = CreateThreads(numberReaders, Read, threadsInfo);
        JoinThreads(threads);
        time_point<system_clock> end = system_clock::now();

        isSuccess &= CheckTestReaders(*set, array);
        timeTotal += duration_cast<milliseconds>(end - start).count() / 1000.0;

        delete set;
    }

    PrintTestResult("test readers", setType, dataType, isSuccess, timeTotal / numberTimes);
}

void RunTestGeneral(SetType setType, DataType dataType, int numberWriters, int numberReaders, int numberRecords, int numberReadings, int numberTimes)
{
    bool isSuccess = true;
    double timeTotal = 0;

    for (int i = 0; i < numberTimes; i++)
    {
        Set<int> *set = CreateSet(setType);
        vector<int> data = CreateData(dataType, numberWriters * numberRecords);
        vector<int> array = vector<int>(numberWriters * numberRecords, 0);

        vector<ThreadInfo> writersInfo = CreateThreadsInfo(numberWriters, set, &data, nullptr, numberWriters, 0, numberRecords, 0);
        vector<ThreadInfo> readersInfo = CreateThreadsInfo(numberReaders, set, &data, &array, 0, numberReaders, 0, numberReadings);

        time_point<system_clock> start = system_clock::now();
        vector<pthread_t> writers = CreateThreads(numberWriters, Write, writersInfo);
        vector<pthread_t> readers = CreateThreads(numberReaders, Read, readersInfo);
        JoinThreads(writers);
        JoinThreads(readers);
        time_point<system_clock> end = system_clock::now();

        isSuccess &= CheckTestReaders(*set, array);
        timeTotal += duration_cast<milliseconds>(end - start).count() / 1000.0;

        delete set;
    }

    PrintTestResult("test general", setType, dataType, isSuccess, timeTotal / numberTimes);
}
