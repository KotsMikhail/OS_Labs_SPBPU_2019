#ifndef __TEST_H__
#define __TEST_H__

enum class TESTING_SETS
{
    FINE_GRAINED = 0,
    LAZY_SYNCHRONIZATION
};


enum class TEST_TYPE
{
    WRITERS_TEST = 0,
    READERS_TEST,
    GENERAL_TEST
};


void runTest     (const TESTING_SETS& setType, const TEST_TYPE& testType);
void runTimeTest ();

#endif //__TEST_H__
