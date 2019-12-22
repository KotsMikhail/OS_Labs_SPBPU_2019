//
// Created by misha on 12.12.2019.
//

#include "Timer.h"

void Timer::Start() {
    clock_gettime(CLOCK_REALTIME, &start_time);
}

void Timer::Stop() {
    struct timespec cur_time;
    clock_gettime(CLOCK_REALTIME, &cur_time);
    result_sec = cur_time.tv_sec - start_time.tv_sec;
    //std::cout << "[" << cur_time.tv_nsec << " \t" << start_time.tv_nsec << "\t" <<  nano <<  "\t" << cur_time.tv_nsec - start_time.tv_nsec << "]" << std::endl;
    result_nsec = (cur_time.tv_nsec - start_time.tv_nsec);
}

unsigned int Timer::GetSec() {
    return result_sec;
}

long int Timer::GetNsec() {
    return result_nsec;
}

double Timer::GetTime() {
    //std::cout << "[" << result_sec << " \t" << result_nsec  << "]" << std::endl;
    return result_sec + (double) result_nsec / nano;
}
