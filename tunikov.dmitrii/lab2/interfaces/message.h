//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_MESSAGE_H
#define LAB2_MESSAGE_H

#include <iostream>
#include <utility>

struct Date
{
    int m_day, m_month, m_year;
    Date(int day = 0, int month = 0, int year = 0) : m_day(day), m_month(month), m_year(year){}
};

struct Message
{
    Date m_date;
    int m_temperature;

    Message(Date date = {}, int temp = 0)
    : m_date(date), m_temperature(temp)
    {
    }
};

#endif //LAB2_MESSAGE_H
