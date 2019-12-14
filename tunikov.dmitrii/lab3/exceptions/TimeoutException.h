//
// Created by dmitrii on 14.12.2019.
//

#ifndef LAB3_TIMEOUTEXCEPTION_H
#define LAB3_TIMEOUTEXCEPTION_H


#include <stdexcept>

class TimeoutException : public std::runtime_error {
public:
    TimeoutException(const char* mess) : std::runtime_error(mess){
    }
};


#endif //LAB3_TIMEOUTEXCEPTION_H
