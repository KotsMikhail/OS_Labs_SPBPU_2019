#ifndef __SIMPLE_DATA_SETS_CREATOR_H__
#define __SIMPLE_DATA_SETS_CREATOR_H__

#include "test_utils.h"

class SIMPLE_DATA_SET_CREATOR : public DATA_SET_CREATOR
{
public:
    SIMPLE_DATA_SET_CREATOR  ();
    ~SIMPLE_DATA_SET_CREATOR () = default;

    bool Create (SET<int> *set, int numOfThreads, const std::vector<int> &numOfActionsPerThread,
                 std::vector<std::vector<int>> &dataSets, std::vector<TEST_INFO *> &testInfos,
                 bool addDataToSet = false);
};

#endif //__SIMPLE_DATA_SETS_CREATOR_H__
