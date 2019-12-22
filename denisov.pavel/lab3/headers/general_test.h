#ifndef __GENERAL_TEST_H__
#define __GENERAL_TEST_H__

#include "test_utils.h"


class GENERAL_TEST_RUNNER : public TEST_RUNNER
{
public:
    GENERAL_TEST_RUNNER  ();
    ~GENERAL_TEST_RUNNER () {}

    bool Run                (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck);
    bool RunWithTimeMeasure (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time);

private:
    bool RunTest (SET<int> *set, DATA_SET_CREATOR *dataSetCreator, bool needCheck, double *time);
    bool Check   (int *checkArr, int numOfRecords);
};

#endif //__GENERAL_TEST_H__
