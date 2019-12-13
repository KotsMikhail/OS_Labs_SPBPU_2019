//
// Created by misha on 10.12.2019.
//

#include "BlockedStack.h"
#include <exception>
#include "IStack.h"
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>

void BlockedStack::push(int const &data) {
    // блокируем на время всей функции

    timed_lock();
    //pthread_mutex_lock(&my_lock);
    node *const new_node = new node(data);
    new_node->next = head;
    //sleep(1);
    head = new_node;
    pthread_mutex_unlock(&my_lock);
}

void BlockedStack::pop(int &result) {
    // блокируем на время всей функции
    timed_lock();
    //pthread_mutex_lock(&my_lock);
    // в начале проверим, не пустой ли стек
    if (head == nullptr) {
        pthread_mutex_unlock(&my_lock);
        throw empty_stack();
    }
    node *old_head = head;

    head = old_head->next;
    result = old_head->data;
    free(old_head);
    pthread_mutex_unlock(&my_lock);
}

void BlockedStack::timed_lock() {
    struct timespec deadline;
    struct timespec cur_time;
    bool init_flag = true;

    while (pthread_mutex_trylock(&my_lock)) {
        if (init_flag) {
            clock_gettime(CLOCK_REALTIME, &deadline);
            deadline.tv_sec += TIMEOUT;
            init_flag = false;
        }
        clock_gettime(CLOCK_REALTIME, &cur_time);
        if (cur_time.tv_sec > deadline.tv_sec) {
            throw timeout_stack();
        }
    }
}

BlockedStack::~BlockedStack() {
    pthread_mutex_destroy(&my_lock);
    int value;
    while (1) {
        try {
            pop(value);
        }
        catch (IStack::empty_stack &e) {
            break;
        } catch (std::exception &e) {
            std::cout << "ERROR : " << e.what() << std::endl;
        }
    }
}

bool BlockedStack::empty() {
    pthread_mutex_lock(&my_lock);
    bool res;
    res = head == 0;
    pthread_mutex_unlock(&my_lock);
    return res;
}

