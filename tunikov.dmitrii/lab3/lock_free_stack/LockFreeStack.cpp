//
// Created by dmitrii on 15.12.2019.
//

#include <new>
#include "LockFreeStack.h"

void LockFreeStack::push(int const& data) {
    CounterNodePtr new_node{};
    new_node.m_ptr = new LockFreeNode(data);
    new_node.m_external_count = 1;
    new_node.m_ptr->m_next = head.load(std::memory_order_relaxed);
    while (!head.compare_exchange_weak(new_node.m_ptr->m_next, new_node, std::memory_order_release, std::memory_order_relaxed));
}

void LockFreeStack::increase_head_count(CounterNodePtr& old_counter) {
    CounterNodePtr new_counter{};
    do {
        new_counter = old_counter;
        ++new_counter.m_external_count;
    }
    while (!head.compare_exchange_strong(old_counter, new_counter,std::memory_order_acquire,std::memory_order_relaxed));

    old_counter.m_external_count = new_counter.m_external_count;
}

std::shared_ptr<int> LockFreeStack::pop() {
    CounterNodePtr old_head = head.load(std::memory_order_relaxed);

    while(true)
    {
        increase_head_count(old_head);
        LockFreeNode* const ptr = old_head.m_ptr;
        if (!ptr) {
            return std::shared_ptr<int>();
        }

        if (head.compare_exchange_strong(old_head, ptr->m_next, std::memory_order_relaxed)) {
            std::shared_ptr<int> res;
            res.swap(ptr->m_data);
            int const count_increase = old_head.m_external_count;
            if (ptr->m_internal_count.fetch_add(count_increase, std::memory_order_release) == -count_increase) {
                delete ptr;
            }
            return res;
        }
        else if (ptr->m_internal_count.fetch_add(-1, std::memory_order_relaxed) == 1) {
            ptr->m_internal_count.load(std::memory_order_acquire);
            delete ptr;
        }
    }
}

LockFreeStack::~LockFreeStack() {
    while(LockFreeStack::pop());
}

LockFreeStack::LockFreeStack() = default;

bool LockFreeStack::empty() {
    CounterNodePtr temp{};
    temp = head.load(std::memory_order_acquire);
    return temp.m_ptr == nullptr;
}
