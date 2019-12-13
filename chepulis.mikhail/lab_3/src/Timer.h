//
// Created by misha on 12.12.2019.
//

#ifndef LAB_3_STACK__TIMER_H
#define LAB_3_STACK__TIMER_H


#include <cstdlib>
#include <iostream>
static const unsigned int nano = 1000000000;

class Timer {
private:
    struct timespec start_time;
    unsigned int result_sec;
    long int result_nsec;
public:
    Timer() {}

    ~Timer() {}

    void Start();

    void Stop();

    unsigned int GetSec();

    long int GetNsec();

    double GetTime();
};


#endif //LAB_3_STACK__TIMER_H
