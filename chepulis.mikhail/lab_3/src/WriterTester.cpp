//
// Created by misha on 11.12.2019.
//

#include "WriterTester.h"
#include "Timer.h"
#include <iostream>
#include <pthread.h>

void *WriterTester::Write(void *arg) {

    thread_args *args = (thread_args *) arg;
    IStack *stack = args->stack;
    int N = args->N;
    bool is_need_print = args->is_need_print;
    int id = args->id;
    int i = 0;
    bool *start_flag = args->start_flag;
    while (!(*start_flag));

    while (i < N) {
        try {
            stack->push(i);
            i++;
        } catch (IStack::empty_stack &e) {
            if (is_need_print) {
                std::cout << id << " : " << e.what() << std::endl;
            }
            break;
        } catch (IStack::timeout_stack &e) {
            if (is_need_print) {
                //std::cout << id << " : " << e.what() << std::endl;
                //писатель выводит сообщение, о превышении таймаута и вновь пытается положить число в стек
                std::cout << id << " : access timed out" << std::endl;
            }
            //break;

            pthread_yield();
        }
    }
    return nullptr;
}

bool WriterTester::Test(IStack *my_stack, int num_of_threads, int num_of_elements, Timer* timer, bool is_need_print) {
    if (num_of_threads == 0 || num_of_elements == 0) {
        std::cout << "num_of_threads == 0 or num_of_elements == 0" << std::endl;
        return true;
    }
    bool result = true;
    pthread_t *threads =  new pthread_t[num_of_threads];


    array =  new int[num_of_elements];
    for (int i = 0; i < num_of_elements; i++) {
        array[i] = 0;
    }
    thread_args *arg_array = new thread_args[num_of_threads];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    void *(*function)(void *) = Write;

    bool start_flag = false;

    for (int i = 0; i < num_of_threads; i++) {
        arg_array[i] = thread_args(my_stack, num_of_elements, is_need_print, i, &start_flag);
        pthread_create(&(threads[i]), &attr, function, &(arg_array[i]));
    }

    if (timer) {
        timer->Start();
    }
    start_flag = true;
    for (int i = 0; i < num_of_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    if (timer) {
        timer->Stop();
    }


    int value;
    while (1) {
        try {
            my_stack->pop(value);
            array[value] += 1;
        } catch (IStack::empty_stack &e) {
            if (is_need_print) {
                //std::cout << pthread_self() << " : "  << e.what() << std::endl;
            }
            break;
        } catch (IStack::timeout_stack &e) {
            if (is_need_print) {
                std::cout << pthread_self() << " : " << e.what() << std::endl;
            }
            break;
        }
    }

    int expected_value = num_of_threads;
    int cur_value;
    for (int i = 0; i < num_of_elements; i++) {
        cur_value = array[i] - expected_value;
        if (is_need_print) {
            std::cout << i << ") " << cur_value << std::endl;
        }
        if (cur_value != 0)
            result = false;
    }

    delete[] threads;
    delete[] array ;
    delete[] arg_array;
    return result;
}
