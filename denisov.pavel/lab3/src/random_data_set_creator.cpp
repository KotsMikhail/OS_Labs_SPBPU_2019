#include <random>
#include <algorithm>

#include "random_data_set_creator.h"


RANDOM_DATA_SET_CREATOR::RANDOM_DATA_SET_CREATOR ()
{
    this->id = AVAILABLE_DATA_SET_CREATOR_ID::RANDOM;
}


bool RANDOM_DATA_SET_CREATOR::Create(SET<int> *set, int numOfThreads,
                                     const std::vector<int> &numOfActionsPerThread,
                                     std::vector<std::vector<int>> &dataSets,
                                     std::vector<TEST_INFO *> &testInfos,
                                     bool addDataToSet)
{
    std::vector<int> elems(std::accumulate(numOfActionsPerThread.begin(), numOfActionsPerThread.end(), 0));
    std::iota(elems.begin(), elems.end(), 0);
    std::shuffle(elems.begin(), elems.end(), std::default_random_engine(0));
    for (int i = 0, k = 0; i < numOfThreads; i++) {
        int n = numOfActionsPerThread[i];
        dataSets[i].resize(n);
        for (int j =0; j < n; j++, k++) {
            dataSets[i][j] = elems[k];
            if (addDataToSet) {
                if (!set->Add(dataSets[i][j])) {
                    delTestInfos(testInfos);

                    std::cout << "Failed in creating random data set. Not enough memory." << std::endl;
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
