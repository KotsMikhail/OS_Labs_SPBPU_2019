//
// Created by dmitrii on 15.12.2019.
//

#include <new>
#include "LockFreeStack.h"
#include "../utils/exceptions.h"

void LockFreeStack::push(int const& data) {
    CounterNodePtr new_node{};
    new_node.m_ptr = new LockFreeNode(data);
    new_node.m_external_count = 1;
    __atomic_load(&head, &new_node.m_ptr->m_next, __ATOMIC_RELAXED);

    while (!__atomic_compare_exchange(&head, &new_node.m_ptr->m_next, &new_node, true, __ATOMIC_RELEASE,
                                       __ATOMIC_RELAXED));

}

void LockFreeStack::increase_head_count(CounterNodePtr& old_counter) {
    CounterNodePtr new_counter{};
    do {
        new_counter = old_counter;
        ++new_counter.m_external_count;
    }
    while (!__atomic_compare_exchange(&head, &old_counter, &new_counter, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED));

    old_counter.m_external_count = new_counter.m_external_count;
}

int LockFreeStack::pop() {
    CounterNodePtr old_head{};
    __atomic_load(&head, &old_head, __ATOMIC_RELAXED);
    while(true)
    {
        increase_head_count(old_head);
        LockFreeNode* const ptr = old_head.m_ptr;
        if (!ptr) {
            throw EmptyStackException("stack is empty");
        }

        if (__atomic_compare_exchange(&head, &old_head, &ptr->m_next, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
            int res = ptr->m_data;
            int const count_increase = old_head.m_external_count - 2;
            if (__atomic_fetch_add(&ptr->m_internal_count, count_increase, __ATOMIC_RELEASE) == -count_increase) {
                delete ptr;
            }
            return res;
        }
        else if (__atomic_fetch_add(&ptr->m_internal_count, -1, __ATOMIC_RELAXED) == 1) {
            __atomic_load_n(&ptr->m_internal_count, __ATOMIC_ACQUIRE);
            delete ptr;
        }
    }
}

LockFreeStack::~LockFreeStack() {
    while(!LockFreeStack::empty())
    {
        LockFreeStack::pop();
    }
}

bool LockFreeStack::empty() {
    CounterNodePtr tmp_head{};
    __atomic_load(&head, &tmp_head, __ATOMIC_ACQUIRE);
    return tmp_head.m_ptr == nullptr;
}
