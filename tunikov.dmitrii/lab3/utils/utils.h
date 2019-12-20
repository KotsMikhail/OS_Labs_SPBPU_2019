//
// Created by dmitrii on 15.12.2019.
//

#ifndef LAB3_UTILS_H
#define LAB3_UTILS_H

#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <sys/resource.h>
#include <iostream>
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

    static void printVectors(const std::vector<std::vector<int>> &vv)
    {
        for (unsigned i = 0; i < vv.size(); i++)
        {
            std::cout << "[" << i << "]: ";
            for (unsigned j = 0; j < vv[i].size(); j++)
            {
                std::cout << vv[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    static float getAverage(const std::vector<unsigned long>& v)
    {
        unsigned long res = 0;

        for (const auto& elem : v)
        {
            res += elem;
        }

        return float(res) / v.size();
    }
};


#endif //LAB3_UTILS_H
