//
// Created by misha on 11.12.2019.
//

#ifndef LAB_3_STACK__COMMONTESTER_H
#define LAB_3_STACK__COMMONTESTER_H


#include <vector>
#include "IStack.h"
#include "Timer.h"


//// заполняем массив длинной в num_of_elements нулями
//// создаём num_of_reader потоков читателей
//// создаём num_of_writer потоков писателей, каждый писатель записывает в стек индексы в своём индивидуально диапазоне
//// разрешаем потокам начать работу
//// после того, как все писатели зокончили работу, выстаялем флаг, что писатели закончили, а значит новые элементы в стеке больше не появятся
//// Ожидание: после того, как стек опустеет все элементы массива array должны равняться 1
//// по таймауту обращания к стеку вызывается pthread_yield();

class CommonTester {

public:
    CommonTester() {}

    ~CommonTester() {}


    bool Test(IStack *my_stack, int num_of_reader, int num_of_writer, int num_of_elements, Timer *timer = nullptr,
              bool is_need_print = false);;

private:
    static void *Read(void *arg);

    static void *Write(void *arg);

    int *array;

    struct thread_W_args {
        int id;
        IStack *stack;
        int start_index;
        int end_index;
        int *array;
        bool is_need_print;
        bool *start_flag;

        thread_W_args(IStack *stack_, int *array_, int start_index_, int end_index_, bool is_need_print_, int id_,
                      bool *start_flag_) :
                id(id_),
                stack(stack_),
                start_index(start_index_),
                end_index(end_index_),
                array(array_),
                is_need_print(is_need_print_),
                start_flag(start_flag_) {}
    };


    struct thread_R_args {
        int id;
        IStack *stack;
        int *array;
        bool is_need_print;
        bool *start_flag;
        bool *writers_work_done;

        thread_R_args(IStack *stack_, int *array_, bool is_need_print_, int id_, bool *start_flag_,
                      bool *writers_work_done_) :
                id(id_),
                stack(stack_),
                array(array_),
                is_need_print(is_need_print_),
                start_flag(start_flag_),
                writers_work_done(writers_work_done_) {}

    };


    std::vector<std::pair<int, int>> GetSectors(int length, int num_of_thread);

};


#endif //LAB_3_STACK__COMMONTESTER_H
