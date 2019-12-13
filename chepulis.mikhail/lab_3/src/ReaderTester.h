//
// Created by misha on 11.12.2019.
//

#ifndef LAB_3_STACK__READERTESTER_H
#define LAB_3_STACK__READERTESTER_H


#include <pthread.h>
#include "IStack.h"
#include "Timer.h"

//// заполняем массив длинной в num_of_elements нулями
//// помещаем в стек все индексы массива array
//// каждый раз считывая значение из стека ингрементируем элемент с этим индексом
//// Ожидание: после того, как стек опустеет все элементы массива array должны равняться 1
//// по таймауту обращания к стеку вызывается pthread_yield();
class ReaderTester {
public:
    ReaderTester() {}

    ~ReaderTester() {}

    bool
    Test(IStack *my_stack, int num_of_threads, int num_of_elements, Timer *timer = nullptr, bool is_need_print = false);

private:
    int *array;

    static void *Read(void *arg);

    struct thread_args {
        int id;
        IStack *stack;
        int *array;
        bool is_need_print;
        bool *start_flag;

        thread_args(IStack *stack_, int *arr_, bool is_need_print_, int id_, bool *start_flag_) :
                id(id_),
                stack(stack_),
                array(arr_),
                is_need_print(is_need_print_),
                start_flag(start_flag_) {}
    };

};


#endif //LAB_3_STACK__READERTESTER_H
