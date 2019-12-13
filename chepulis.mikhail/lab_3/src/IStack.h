//
// Created by misha on 10.12.2019.
//

#ifndef LAB_3_STACK__ISTACK_H
#define LAB_3_STACK__ISTACK_H

#include <exception>
#define  TIMEOUT 2

class IStack {
public:
    IStack() {}

    ~IStack() {}

    virtual void push(int const &data) {}

    virtual void pop(int &result) {}

    virtual bool empty() { return false; }

    struct empty_stack : std::exception {
        const char *what() const throw() {
            return "stack is empty";
        }
    };

    struct timeout_stack : std::exception {
        const char *what() const throw() {
            return "waiting time is over";
        }
    };

};

#endif //LAB_3_STACK__ISTACK_H
