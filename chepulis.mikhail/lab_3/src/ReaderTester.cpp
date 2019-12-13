//
// Created by misha on 11.12.2019.
//

#include "ReaderTester.h"
#include "Timer.h"
#include <iostream>
#include <pthread.h>

bool ReaderTester::Test(IStack* my_stack, int num_of_threads, int num_of_elements, Timer* timer, bool is_need_print) {

    if (num_of_threads == 0 || num_of_elements == 0) {
        std::cout << "num_of_threads == 0 or num_of_elements == 0" << std::endl;
        return true;
    }

    bool result = true;
    pthread_t *threads = (pthread_t *) malloc(num_of_threads * sizeof(pthread_t));

    for (int i = 0; i < num_of_elements; i++) {
        my_stack->push(i);
    }
    array = (int *) malloc(num_of_elements * sizeof(int));
    for (int i = 0; i < num_of_elements; i++) {
        array[i] = 0;
    }

    thread_args *arg_array = (thread_args *) malloc(num_of_threads * sizeof(thread_args));
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    void *(*function)(void *) = Read;

    bool start_flag = false;
    for (int i = 0; i < num_of_threads; i++) {
        arg_array[i] = thread_args(my_stack, array, is_need_print, i, &start_flag);
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

    free(threads);
    free(array);
    free(arg_array);

    return result;
}

void *ReaderTester::Read(void *arg) {

    thread_args *args = (thread_args *) arg;
    IStack *stack = args->stack;
    int *array = args->array;
    bool is_need_print = args->is_need_print;
    int id = args->id;
    bool *start_flag = args->start_flag;
    while (!(*start_flag));

    int value;
    while (1) {
        try {
            stack->pop(value);
            array[value] += 1;
            if (is_need_print) {
                std::cout << id << " : " << "increment (" << value << ")  = " << array[value] << std::endl;
            }
            //std::cout << pthread_self() << " : " << "increment (" << value<< ")  = " << array[value] << std::endl;
        } catch (IStack::empty_stack &e) {
            if (is_need_print) {
                std::cout << id << " : " << e.what() << std::endl;
            }
            break;
        } catch (IStack::timeout_stack &e) {
            if (is_need_print) {
                std::cout << id << " : " << e.what() << std::endl;
            }
            std::cout << id << " : access timed out" << std::endl;
            pthread_yield();
        }
    }
    return nullptr;

}
