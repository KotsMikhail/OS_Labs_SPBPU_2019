//
// Created by misha on 10.12.2019.
//

#include "NonBlockedStack.h"
#include <pthread.h>
#include <cstdlib>
#include <iostream>

void NonBlockedStack::increase_head_count(NonBlockedStack::CountedNodePtr &old_counter) {
    CountedNodePtr new_counter;

    struct timespec timer;
    bool flag = true;
    do {
        new_counter = old_counter;
        ++new_counter.external_count;
        if (IsTimeout(&timer, flag)) {
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            throw timeout_stack();
        }
        flag = false;
    } while (!__atomic_compare_exchange(&head, &old_counter, &new_counter, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED));
    //} while (!__atomic_compare_exchange(&head, &old_counter, &new_counter, true, __ATOMIC_RELEASE, __ATOMIC_RELAXED));


    old_counter.external_count = new_counter.external_count;
}

NonBlockedStack::~NonBlockedStack() {

    int value;
    while (1) {
        try {
            pop(value);
        }
        catch (IStack::empty_stack &e) {
            break;
        } catch (std::exception &e) {
            std::cout << "ERROR D-TOR non block : " << e.what() << std::endl;
        }
    }
}

void NonBlockedStack::push(int const &data) {

    CountedNodePtr new_node;
    new_node.ptr = new Node(data);
    new_node.external_count = 1;
    __atomic_load(&head, &new_node.ptr->next, __ATOMIC_RELAXED);

    struct timespec timer;
    bool flag = true;
    while (!__atomic_compare_exchange(&head, &new_node.ptr->next, &new_node, true, __ATOMIC_RELEASE,
                                      __ATOMIC_RELAXED)) {
        if (IsTimeout(&timer, flag)) {
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            free(new_node.ptr);
            throw timeout_stack();
        }
        flag = false;
    }
}

void NonBlockedStack::pop(int &result) {

    CountedNodePtr old_head;
    __atomic_load(&head, &old_head, __ATOMIC_RELAXED);
    struct timespec timer;
    bool flag = true;

    while (true) {
        increase_head_count(old_head);
        Node *const ptr = old_head.ptr;
        if (!ptr) {
            throw empty_stack();
        }


        if (__atomic_compare_exchange(&head, &old_head, &ptr->next, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
            result = ptr->data;
            int const count_increase = old_head.external_count - 2;

            if (__atomic_fetch_add(&ptr->internal_count, count_increase, __ATOMIC_RELEASE) == -count_increase) {
                delete ptr;
            }
            return;
        } else if (__atomic_fetch_add(&ptr->internal_count, -1, __ATOMIC_RELAXED) == 1) {
            __atomic_load_n(&ptr->internal_count, __ATOMIC_ACQUIRE);
            delete ptr;
        }
        if (IsTimeout(&timer, flag)) {
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            throw timeout_stack();
        }
        flag = false;
    }
}

bool NonBlockedStack::empty() {
    CountedNodePtr temp;
    __atomic_load(&head, &temp, __ATOMIC_ACQUIRE);
    return temp.ptr == 0;
}



bool NonBlockedStack::IsTimeout(timespec *timer, bool flag) {
    struct timespec cur_time;
    if (flag) {
        clock_gettime(CLOCK_REALTIME, timer);
    }
    clock_gettime(CLOCK_REALTIME, &cur_time);
    if (cur_time.tv_sec - timer->tv_sec > TIMEOUT) {
        return true;
        //throw timeout_stack();
    }
    return false;
}
