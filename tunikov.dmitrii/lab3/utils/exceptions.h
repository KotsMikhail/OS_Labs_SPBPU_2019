//
// Created by dmitrii on 19.12.2019.
//

#ifndef LAB3_EXCEPTIONS_H
#define LAB3_EXCEPTIONS_H

#include <stdexcept>

class EmptyStackException : public std::runtime_error
{
public:
    explicit EmptyStackException(const char* mess) : std::runtime_error(mess) {}
};


#endif //LAB3_EXCEPTIONS_H
