//
// Created by dmitrii on 15.12.2019.
//

#ifndef LAB3_UTILS_H
#define LAB3_UTILS_H

#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <sys/resource.h>
#include <stdexcept>
#include <ctime>


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
                if (vv[i].empty())
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

    static int getMaxThreadsCount()
    {
        struct rlimit rlim{};
        int rc = getrlimit(RLIMIT_NPROC, &rlim);
        if (rc == 0)
            return rlim.rlim_max;
        else
            throw std::runtime_error("can't get max threads count");
    }
};


#endif //LAB3_UTILS_H
