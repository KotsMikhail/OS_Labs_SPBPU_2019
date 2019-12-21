//
// Created by misha on 13.12.2019.
//

#ifndef LAB_3_STACK__TESTDATA_H
#define LAB_3_STACK__TESTDATA_H

#include <string>
#include <iostream>

struct TestData {
    int R_num;
    int W_num;
    int N;
    bool result;
    double time;
    int repeat;

    TestData() :
            R_num(0),
            W_num(0),
            N(0),
            result(false),
            time(0.0),
            repeat(0) {}


    TestData(int R_num_, int W_num_, int N_, bool result_, double time_, int repeat_) :
            R_num(R_num_),
            W_num(W_num_),
            N(N_),
            result(result_),
            time(time_),
            repeat(repeat_) {}
};

template <typename T>
std::string GetCeil(T var);
std::string GetCeil(const char* var);
std::string GetCeil(const bool var);
void PrintDataHeader();
void PrintData(TestData data);
void PrintTitle(int k);

#endif //LAB_3_STACK__TESTDATA_H
