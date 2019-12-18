#include <iostream>

#include "tests.h"

using namespace std;

const int NUMBER_TIMES = 10;

int CheckArgument(char *arg)
{
    int number = atoi(arg);

    if (number <= 0)
    {
        cout << "Wrong argument: found " << arg << ", expected number >= 1." << endl;
        cout << "Usage: ./run_tests number_of_writers number_of_readers number_of_records number_of_readings." << endl;
        cout << "For example: ./run_tests 4 4 250 250." << endl;

        exit(EXIT_FAILURE);
    }
    
    return number;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        cout << "Wrong number of arguments: found " << argc - 1 << ", expected 4." << endl;
        cout << "Usage: ./run_tests number_of_writers number_of_readers number_of_records number_of_readings." << endl;
        cout << "For example: ./run_tests 4 4 250 250." << endl;

        return EXIT_FAILURE;
    }

    int numberWriters = CheckArgument(argv[1]);
    int numberReaders = CheckArgument(argv[2]);
    int numberRecords = CheckArgument(argv[3]);
    int numberReadings = CheckArgument(argv[4]);

    if (numberWriters * numberRecords != numberReaders * numberReadings)
    {
        cout << "Wrong arguments: number_of_writers * number_of_records != number_of_readers * number_of_readings." << endl;
        cout << "Usage: ./run_tests number_of_writers number_of_readers number_of_records number_of_readings." << endl;
        cout << "For example: ./run_tests 4 4 250 250." << endl;

        return EXIT_FAILURE;
    }

    RunTestWriters(SetType::FINE_LIST, DataType::RANDOM, numberWriters, numberRecords, NUMBER_TIMES);
    RunTestWriters(SetType::OPTIMISTIC_LIST, DataType::RANDOM, numberWriters, numberRecords, NUMBER_TIMES);
    RunTestWriters(SetType::FINE_LIST, DataType::FIXED, numberWriters, numberRecords, NUMBER_TIMES);
    RunTestWriters(SetType::OPTIMISTIC_LIST, DataType::FIXED, numberWriters, numberRecords, NUMBER_TIMES);

    RunTestReaders(SetType::FINE_LIST, DataType::RANDOM, numberReaders, numberReadings, NUMBER_TIMES);
    RunTestReaders(SetType::OPTIMISTIC_LIST, DataType::RANDOM, numberReaders, numberReadings, NUMBER_TIMES);
    RunTestReaders(SetType::FINE_LIST, DataType::FIXED, numberReaders, numberReadings, NUMBER_TIMES);
    RunTestReaders(SetType::OPTIMISTIC_LIST, DataType::FIXED, numberReaders, numberReadings, NUMBER_TIMES);

    RunTestGeneral(SetType::FINE_LIST, DataType::RANDOM, numberWriters, numberReaders, numberRecords, numberReadings, NUMBER_TIMES);
    RunTestGeneral(SetType::OPTIMISTIC_LIST, DataType::RANDOM, numberWriters, numberReaders, numberRecords, numberReadings, NUMBER_TIMES);
    RunTestGeneral(SetType::FINE_LIST, DataType::FIXED, numberWriters, numberReaders, numberRecords, numberReadings, NUMBER_TIMES);
    RunTestGeneral(SetType::OPTIMISTIC_LIST, DataType::FIXED, numberWriters, numberReaders, numberRecords, numberReadings, NUMBER_TIMES);

    return 0;
}
