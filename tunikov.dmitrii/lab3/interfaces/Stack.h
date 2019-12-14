//
// Created by dmitrii on 14.12.2019.
//

#ifndef LAB3_STACK_H
#define LAB3_STACK_H

class Stack
{
public:
    static int m_read_timeout;
    virtual void pop(int& val) = 0;
    virtual bool empty() = 0;
    virtual void push(const int& val) = 0;
};

#endif //LAB3_STACK_H
