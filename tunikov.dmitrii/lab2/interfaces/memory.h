//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_MEMORY_H
#define LAB2_MEMORY_H

#include <iostream>
#include <utility>

enum MemoryOwner
{
    HOST,
    CLIENT
};

struct Date
{
    int m_day, m_month, m_year;
    Date(int day = 0, int month = 0, int year = 0) : m_day(day), m_month(month), m_year(year){}

    void printDate(const std::string& msg)
    {
        std::cout << msg << ": " << m_day << "." << m_month << "." << m_year << std::endl;
    }
};

struct Memory
{
    MemoryOwner m_owner;
    Date m_date;
    int m_temperature;

    Memory(Date date = Date(), int temp = 0, MemoryOwner owner = HOST)
    : m_owner(owner), m_date(date), m_temperature(temp)
    {
    }
};

#endif //LAB2_MEMORY_H
