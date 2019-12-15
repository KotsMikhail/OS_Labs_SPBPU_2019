//
// Created by dmitrii on 15.12.2019.
//

#ifndef LAB3_UTILS_H
#define LAB3_UTILS_H

#include <vector>
#include <pthread.h>

class utils {
public:
    static bool containsAndErase(std::vector<int>& vec, const int& val)
    {
        for (unsigned k = 0; k < vec.size(); k++)
        {
            if (vec[k] == val)
            {
                vec.erase(vec.begin() + k);
                return true;
            }
        }
        return false;
    }

    static bool containsAndErase(std::vector<std::vector<int>>& vv, const int& val)
    {
        for (unsigned i = 0; i < vv.size(); i++)
        {
            bool was_found = utils::containsAndErase(vv[i], val);
            if (was_found)
            {
                if (vv[i].size() == 0)
                    vv.erase(vv.begin() + i);
                return true;
            }
        }
        return false;
    }

    static void joinThreads(std::vector<pthread_t>& threads)
    {
        for (unsigned i = 0; i < threads.size(); i++)
            pthread_join(threads[i],nullptr);
    }
};


#endif //LAB3_UTILS_H
