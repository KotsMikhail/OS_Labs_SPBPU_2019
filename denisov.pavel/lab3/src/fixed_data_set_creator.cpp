#include <algorithm>

#include "fixed_data_set_creator.h"


FIXED_DATA_SET_CREATOR::FIXED_DATA_SET_CREATOR ()
{
    this->id = AVAILABLE_DATA_SET_CREATOR_ID::FIXED;
}


bool FIXED_DATA_SET_CREATOR::Create(SET<int> *set, int numOfThreads,
                                    const std::vector<int> &numOfActionsPerThread,
                                    std::vector<std::vector<int>> &dataSets,
                                    std::vector<TEST_INFO *> &testInfos,
                                    bool addDataToSet)
{
    int maxElems = *std::max_element(numOfActionsPerThread.begin(), numOfActionsPerThread.end());
    std::vector<int> indicies(numOfThreads, 0);
    for (int i = 0; i < numOfThreads; i++) {
        dataSets[i].resize(numOfActionsPerThread[i]);
    }

    int val = 0;
    for (int i = 0; i < maxElems; i++) {
        for (int j = 0; j < numOfThreads; j++) {
            if (indicies[j] != numOfActionsPerThread[j]) {
                dataSets[j][indicies[j]] = val++;
                if (addDataToSet) {
                    if (!set->Add(dataSets[j][indicies[j]])) {
                        delTestInfos(testInfos);

                        std::cout << "Failed in creating random data set. Not enough memory." << std::endl;
                        return false;
                    }
                }

                indicies[j]++;
            }
        }
    }

    for (int i = 0; i < numOfThreads; i++) {
        testInfos[i] = new (std::nothrow) TEST_INFO(set, dataSets[i]);
        if (!testInfos[i]) {
            delTestInfos(testInfos);

            std::cout << "Failed in creating testing information structure for thread. Not enough memory." << std::endl;
            return false;
        }
    }

    return true;
}
