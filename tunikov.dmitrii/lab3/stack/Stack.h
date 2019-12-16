//
// Created by dmitrii on 14.12.2019.
//

#ifndef LAB3_STACK_H
#define LAB3_STACK_H

#include <memory>
#include <vector>

enum StackType
{
    LOCK,
    LOCK_FREE
};

class Stack
{
public:
    static int m_read_timeout;
    virtual std::shared_ptr<int> pop() = 0;
    virtual void push(const int& val) = 0;
    virtual bool empty() = 0;
    static void *readFromStack(void *arg);
    static void* writeToStack(void *arg);
    virtual ~Stack();
};

#endif //LAB3_STACK_H
