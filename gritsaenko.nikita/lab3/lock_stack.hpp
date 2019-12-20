#ifndef LOCK_STACK
#define LOCK_STACK

#include "stack.hpp"
#include <memory>

template<typename T>
class lock_stack_t : public stack_t<T>
{
public:
    lock_stack_t() : stack_top(0)
    {
        pthread_mutex_init(&m, 0);
    }

    ~lock_stack_t()
    {
        pthread_mutex_destroy(&m);
        while (pop());
    }

    void push(T const & new_value)
    {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
        node_t * const new_stack_top = new node_t(new_value);

        pthread_mutex_lock(&m);

        new_stack_top->next = stack_top;
        stack_top = new_stack_top;

        pthread_mutex_unlock(&m);
    }

    std::shared_ptr<T> pop()
    {
        pthread_mutex_lock(&m);

        if (stack_top == NULL)
        {
            pthread_mutex_unlock(&m);
            return std::shared_ptr<T>();
        }

        std::shared_ptr<T> const res = stack_top->data;
        node_t *tmp = stack_top;
        stack_top = stack_top->next;

        pthread_mutex_unlock(&m);
        delete tmp;
        return res;
    }

    bool empty() const
    {
        pthread_mutex_lock(&m);
        bool res = (stack_top == NULL) ? true : false;
        pthread_mutex_unlock(&m);
        return res;
    }

private:
    struct node_t
    {
        std::shared_ptr<T> data;
        node_t* next;
        node_t(T const & data):
            data(std::make_shared<T>(data))
        {}
    };

    node_t* stack_top;
    mutable pthread_mutex_t m;
};

#endif
