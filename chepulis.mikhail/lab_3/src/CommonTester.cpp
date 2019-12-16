//
// Created by misha on 11.12.2019.
//

#include <iostream>
#include "CommonTester.h"
#include <pthread.h>
#include <cmath>


void *CommonTester::Read(void *arg) {
    thread_R_args *args = (thread_R_args *) arg;
    IStack *stack = args->stack;
    int *array = args->array;
    bool is_need_print = args->is_need_print;
    int id = args->id;
    bool *start_flag = args->start_flag;
    bool *writers_work_done = args->writers_work_done;

    while ((*start_flag) == false) {
        //std::cout << "reader wait"<< std::endl;
    }


    int value;
    bool local_flag = *writers_work_done;
    while (1) {
        local_flag = *writers_work_done;
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
            if (local_flag) {
                break;
            }
        } catch (IStack::timeout_stack &e) {
            if (is_need_print) {
                //std::cout << id << " : " << e.what() << std::endl;
                std::cout << "Reader" << id << " : access timed out" << std::endl;
            }

            pthread_yield();
        }
    }
    return nullptr;
}

void *CommonTester::Write(void *arg) {
    thread_W_args *args = (thread_W_args *) arg;
    IStack *stack = args->stack;
    int start_index = args->start_index;
    int end_index = args->end_index;
    bool is_need_print = args->is_need_print;
    int id = args->id;
    bool *start_flag = args->start_flag;

    while ((*start_flag) == false) {
        //std::cout << "writer wait"<< std::endl;
    }

    int i = start_index;
    //while( i <= end_index) {
    while (i < end_index) {
        try {
            stack->push(i);
            i++;
            if (is_need_print) {
                std::cout << id << " : " << "pushed (" << i << ")" << std::endl;
            }
        } catch (IStack::empty_stack &e) {
            if (is_need_print) {
                std::cout << id << " : " << e.what() << std::endl;
            }
            break;
        } catch (IStack::timeout_stack &e) {
            if (is_need_print) {
                //std::cout << id << " : " << e.what() << std::endl;
                //писатель выводит сообщение, о превышении таймаута и вновь пытается положить число в стек
                std::cout << "Writer : " << id << " : access timed out" << std::endl;
            }
            //break;

            pthread_yield();
        }
    }
    return nullptr;
}


bool CommonTester::Test(IStack *my_stack, int num_of_reader, int num_of_writer, int num_of_elements, Timer* timer, bool is_need_print) {

    if (num_of_reader == 0 || num_of_writer == 0 || num_of_elements == 0) {
        std::cout << "num_of_reader== 0 or num_of_writer == 0 or num_of_elements == 0" << std::endl;
        return true;
    }

    bool result = true;
    pthread_t *R_threads = (pthread_t *) malloc((num_of_reader) * sizeof(pthread_t));
    pthread_t *W_threads = (pthread_t *) malloc((num_of_writer) * sizeof(pthread_t));
    thread_W_args *arg_W_array = (thread_W_args *) malloc(num_of_writer * sizeof(thread_W_args));
    thread_R_args *arg_R_array = (thread_R_args *) malloc(num_of_reader * sizeof(thread_R_args));


    array = (int *) malloc(num_of_elements * sizeof(int));
    for (int i = 0; i < num_of_elements; i++) {
        array[i] = 0;
    }


    bool start_threads_flag = false;
    bool writers_work_done = false;
    pthread_attr_t attr;
    pthread_attr_init(&attr);


    //Создаём потоки читателей
    for (int i = 0; i < num_of_reader; i++) {
        arg_R_array[i] = thread_R_args(my_stack, array, is_need_print, i, &start_threads_flag, &writers_work_done);
        pthread_create(&(R_threads[i]), &attr, Read, &(arg_R_array[i]));
    }

    //Создаём потоки писателей
    std::vector<std::pair<int, int>> sectors = GetSectors(num_of_elements, num_of_writer);
    int start, end;
    for (int i = 0; i < num_of_writer; i++) {
        start = sectors[i].first;
        end = sectors[i].second;
        arg_W_array[i] = thread_W_args(my_stack, array, start, end, is_need_print, i, &start_threads_flag);
        pthread_create(&(W_threads[i]), &attr, Write, &(arg_W_array[i]));
    }

    // разрешаем всем потока начать работу
    if (timer) {
        timer->Start();
    }
    start_threads_flag = true;

    // ждём когда все писатели закончат своб работу
    for (int i = 0; i < num_of_writer; i++) {
        pthread_join(W_threads[i], NULL);
    }

    // выстаялем флаг, что писатели закончили, а значит новые элементы в стеке больше не появятся
    writers_work_done = true;


    // ждём когда все читатели закончат своб работу
    for (int i = 0; i < num_of_reader; i++) {
        pthread_join(R_threads[i], NULL);
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

    free(R_threads);

    free(W_threads);
    free(array);
    free(arg_W_array);
    free(arg_R_array);

    return result;
}


std::vector<std::pair<int, int>> CommonTester::GetSectors(int length, int num_of_thread) {
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