//
// Created by misha on 11.12.2019.
//

#ifndef LAB_3_STACK__WRITERTESTER_H
#define LAB_3_STACK__WRITERTESTER_H

#include <pthread.h>
#include "IStack.h"
#include "Timer.h"

//// заполняем массив длинной в num_of_elements нулями
//// каждый поток читателей кладёт в стек почерёдно числа от 1 до num_of_elements-1
//// после того, как все потоки читателей закончатся в цикле снимаем значение со стека и инкрементируем элемент с этим индексом
//// Ожидание: после того, как стек опустеет все элементы массива array должны равняться num_of_threads
//// по таймауту обращания к стеку вызывается pthread_yield();

class WriterTester {


public:
    WriterTester() {}

    ~WriterTester() {}

    bool
    Test(IStack *my_stack, int num_of_threads, int num_of_elements, Timer *timer = nullptr, bool is_need_print = false);

private:
    int *array;

    static void *Write(void *arg);

    struct thread_args {
        int id;
        IStack *stack;
        int N;
        bool is_need_print;
        bool *start_flag;
        thread_args() :
                id(0),
                stack(nullptr),
                N(0),
                is_need_print(false),
                start_flag(nullptr) {}
        thread_args(IStack *stack_, int N_, bool is_need_print_, int id_, bool *start_flag_) :
                id(id_),
                stack(stack_),
                N(N_),
                is_need_print(is_need_print_),
                start_flag(start_flag_) {}
    };


};


#endif //LAB_3_STACK__WRITERTESTER_H
