#ifndef __WRITERS_TEST_H__
#define __WRITERS_TEST_H__

#include "test_utils.h"


class WRITERS_TEST_RUNNER : public TEST_RUNNER
{
public:
    WRITERS_TEST_RUNNER  ();
    ~WRITERS_TEST_RUNNER () {}

    bool Run                (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck);
    bool RunWithTimeMeasure (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time);

private:
    bool RunTest (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time);
    bool Check   (SET<int> *set, std::vector<std::vector<int>> dataSets);
};

#endif //__WRITERS_TEST_H__
