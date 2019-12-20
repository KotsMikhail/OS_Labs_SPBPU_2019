#ifndef STACK_H
#define STACK_H

#include <memory>

template <typename T>
class stack_t
{
public:
    virtual ~stack_t(){};
    virtual std::shared_ptr<T> pop() = 0;
    virtual void push(T const &data) = 0;
    virtual bool empty() const = 0;
};

#endif
