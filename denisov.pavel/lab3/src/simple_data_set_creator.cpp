#include <new>

#include "simple_data_set_creator.h"


SIMPLE_DATA_SET_CREATOR::SIMPLE_DATA_SET_CREATOR ()
{
    this->id = AVAILABLE_DATA_SET_CREATOR_ID::SIMPLE;
}


bool SIMPLE_DATA_SET_CREATOR::Create(SET<int> *set, int numOfThreads,
                                     const std::vector<int> &numOfActionsPerThread,
                                     std::vector<std::vector<int>> &dataSets,
                                     std::vector<TEST_INFO *> &testInfos,
                                     bool addDataToSet)
{
    int val = 0;
    for (int i = 0; i < numOfThreads; i++) {
        int n = numOfActionsPerThread[i];
        dataSets[i].resize(n);
        for (int j = 0; j < n; j++) {
            dataSets[i][j] = val++;
            if (addDataToSet) {
                if (!set->Add(dataSets[i][j])) {
                    delTestInfos(testInfos);

                    std::cout << "Failed in creating simple data set. Not enough memory." << std::endl;
                    return false;
                }
            }
        }

        testInfos[i] = new (std::nothrow) TEST_INFO(set, dataSets[i]);
        if (!testInfos[i]) {
            delTestInfos(testInfos);

            std::cout << "Failed in creating testing information structure for thread. Not enough memory." << std::endl;
            return false;
        }
    }

    return true;
}
