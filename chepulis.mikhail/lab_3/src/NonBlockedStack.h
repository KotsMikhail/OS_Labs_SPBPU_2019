//
// Created by misha on 10.12.2019.
//

#ifndef LAB_3_STACK__NONBLOCKEDSTACK_H
#define LAB_3_STACK__NONBLOCKEDSTACK_H



#include "IStack.h"
#include <cstdlib>

class NonBlockedStack : public IStack {
private:
    struct Node;

    struct CountedNodePtr {
        int external_count;
        Node *ptr;

        CountedNodePtr() : external_count(0), ptr(nullptr) {}

        CountedNodePtr(Node *ptr_) : external_count(0), ptr(ptr_) {}
    }__attribute__((__aligned__(16)));


    struct Node {
        int data;
        int internal_count;
        CountedNodePtr next;

        Node(int const &data_) : data(data_), internal_count(0), next(nullptr) {};
    };

    CountedNodePtr head;

    bool IsTimeout(timespec *timer, bool flag = false);

    void increase_head_count(CountedNodePtr &old_counter);

public:
    NonBlockedStack() : head(nullptr) {}

    ~NonBlockedStack();

    void push(int const &data);

    void pop(int &result);

    bool empty();

};



#endif //LAB_3_STACK__NONBLOCKEDSTACK_H
