#ifndef LOCK_FREE_STACK_HPP
#define LOCK_FREE_STACK_HPP

#include "stack.hpp"
#include <stdio.h>
#include <memory>

template<typename T>
class lock_free_stack_t : public stack_t<T>
{
public:
    ~lock_free_stack_t()
    {
        while(pop());
    }

    void push(T const & data)
    {
        counted_node_ptr new_node;
        new_node.ptr = new node_t(data);
        new_node.external_count = 1;
        __atomic_load(&head, &new_node.ptr->next, __ATOMIC_RELAXED);

        while (!__atomic_compare_exchange(&head, &new_node.ptr->next, &new_node, true, __ATOMIC_RELEASE, __ATOMIC_RELAXED));

    }

    std::shared_ptr<T> pop()
    {
        counted_node_ptr old_head;
        __atomic_load(&head, &old_head, __ATOMIC_RELAXED);

        for (;;)
        {
            increase_head_count(old_head);

            node_t* const ptr = old_head.ptr;
            if (!ptr)
            {
                return std::shared_ptr<T>();
            }

            if (__atomic_compare_exchange(&head, &old_head, &ptr->next, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
            {
                std::shared_ptr<T> res;
                res.swap(ptr->data);
                int const count_increase = old_head.external_count - 2;
                if (__atomic_fetch_add(&ptr->internal_count, count_increase, __ATOMIC_RELEASE) == -count_increase)
                {
                    delete ptr;
                }
                return res;
            }
            else if (__atomic_fetch_add(&ptr->internal_count, -1, __ATOMIC_RELAXED) == 1)
            {
                __atomic_load_n(&ptr->internal_count, __ATOMIC_ACQUIRE);
                delete ptr;
            }
        }
    }

    bool empty() const
    {
        counted_node_ptr temp;
        __atomic_load(&head, &temp, __ATOMIC_ACQUIRE);
        return temp.ptr == 0;
    }
private:
    struct node_t;

    struct counted_node_ptr
    {
        int external_count;
        node_t *ptr;
    }__attribute__((__aligned__(16)));
    
    struct node_t
    {
        std::shared_ptr<T> data;
        int internal_count;
        counted_node_ptr next;
        node_t(T const & data):
            data(std::make_shared<T>(data)),
            internal_count(0)
        {}
    };

    counted_node_ptr head;

    void increase_head_count(counted_node_ptr &old_counter)
    {
        counted_node_ptr new_counter;

        do
        {
            new_counter = old_counter;
            ++new_counter.external_count;
        }
        while(!__atomic_compare_exchange(&head, &old_counter, &new_counter, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED));

        old_counter.external_count = new_counter.external_count;
    }


};

#endif
