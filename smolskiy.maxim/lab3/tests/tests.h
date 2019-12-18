#ifndef TESTS_H
#define TESTS_H

enum class SetType
{
    FINE_LIST,
    OPTIMISTIC_LIST
};

enum class DataType
{
    RANDOM,
    FIXED
};

void RunTestWriters(SetType setType, DataType dataType, int numberWriters, int numberRecords, int numberTimes);
void RunTestReaders(SetType setType, DataType dataType, int numberReaders, int numberReadings, int numberTimes);
void RunTestGeneral(SetType setType, DataType dataType, int numberWriters, int numberReaders, int numberRecords, int numberReadings, int numberTimes);

#endif // TESTS_H
