#ifndef __READERS_TEST_H__
#define __READERS_TEST_H__

#include "test_utils.h"


class READERS_TEST_RUNNER : public TEST_RUNNER
{
public:
    READERS_TEST_RUNNER  ();
    ~READERS_TEST_RUNNER () {}

    bool Run                (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck);
    bool RunWithTimeMeasure (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time);

private:
    bool RunTest (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time);
    bool Check   (SET<int> *set, std::vector<std::vector<int>> dataSets);
};

#endif //__READERS_TEST_H__
