//
// Created by misha on 10.12.2019.
//

#ifndef LAB_3_STEK__BLOCKED_STACK_H
#define LAB_3_STEK__BLOCKED_STACK_H

#include "IStack.h"
#include <pthread.h>



class BlockedStack : public IStack {
public:

private:
    struct node {
        int data;
        node *next;

        node(int const &data_) : data(data_), next(nullptr) {};
    };

    node *head;
    pthread_mutex_t my_lock;

    void timed_lock();

public:
    BlockedStack() : head(nullptr) {}

    ~BlockedStack();

    void push(int const &data);

    void pop(int &result);

    bool empty();
};


#endif //LAB_3_STEK__BLOCKED_STACK_H
