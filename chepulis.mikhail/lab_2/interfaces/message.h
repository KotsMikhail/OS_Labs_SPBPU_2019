//
// Created by misha on 23.11.2019.
//

#ifndef LAB_2_SEVER__MESSAGE_H
#define LAB_2_SEVER__MESSAGE_H

#include <string>

static std::string sem_name = "LAB2";

struct Message
{
    int number;
    int day;
    int month;
    int year;

    Message(int num = 0) :number(num), day(0), month(0), year(0)
    {}
};

const int TIMEOUT = 5;
#endif //LAB_2_SEVER__MESSAGE_H
