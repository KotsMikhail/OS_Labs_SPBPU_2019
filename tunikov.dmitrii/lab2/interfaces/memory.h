//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_MEMORY_H
#define LAB2_MEMORY_H

#include <iostream>
#include <utility>

struct Memory
{
    std::string date;

    Memory(std::string date_ = "") : date(std::move(date_))
    {
    }
};

#endif //LAB2_MEMORY_H
