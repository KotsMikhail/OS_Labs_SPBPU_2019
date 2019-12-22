//
// Created by misha on 11.12.2019.
//

#include "WriterTester.h"
#include "Timer.h"
#include <iostream>
#include <pthread.h>
#include <cmath>

void *WriterTester::Write(void *arg) {

    thread_args *args = (thread_args *) arg;
    IStack *stack = args->stack;
    int start_index = args->start_index;
    int end_index = args->end_index;
    bool is_need_print = args->is_need_print;
    int id = args->id;
    bool *start_flag = args->start_flag;
    while (!(*start_flag));

    for (int i = start_index; i < end_index; i++) {
        try {
            stack->push(i);
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
    pthread_t *threads = new pthread_t[num_of_threads];


    array = new int[num_of_elements];
    for (int i = 0; i < num_of_elements; i++) {
        array[i] = 0;
    }
    thread_args *arg_array = new thread_args[num_of_threads];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    void *(*function)(void *) = Write;

    bool start_flag = false;

    std::vector<std::pair<int, int>> sectors = GetSectors(num_of_elements, num_of_threads);
    int start, end;
    for (int i = 0; i < num_of_threads; i++) {
        start = sectors[i].first;
        end = sectors[i].second;
        arg_array[i] = thread_args(my_stack, start, end, is_need_print, i, &start_flag);
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

    int expected_value = 1;
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
    delete[] array;
    delete[] arg_array;
    return result;
}

std::vector<std::pair<int, int>> WriterTester::GetSectors(int length, int num_of_thread) {
    std::vector<std::pair<int, int>> result;

    if (num_of_thread > length) {
        std::cout << "num of writer > array length : " << num_of_thread << " > " << length << std::endl;
    }

    int cur_length = length;
    int begin = 0;
    int end = 0;
    double cur_step = ((double) length) / num_of_thread;
    int cur_num = num_of_thread;
    for (int i = 0; i < length; i++) {
        cur_step = ceil(((double) cur_length) / cur_num);
        end = begin + cur_step;
        result.emplace_back(std::pair<int, int>(begin, end));
        cur_length -= cur_step;
        cur_num--;
        begin = end;
    }

    return result;
}