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
std::string GetCeil(T var) {
    std::string str = std::to_string(var);
    str.resize(10, ' ');
    return str;
}
std::string GetCeil(const char* var) {
    std::string str = var;
    str.resize(10, ' ');
    return str;
}
std::string GetCeil(const bool var) {
    std::string str = var ? "True" : "False";
    str.resize(10, ' ');
    return str;
}

void PrintDataHeader() {
    std::cout << GetCeil("R_num ") << GetCeil("W_num") << GetCeil("N") << GetCeil("result") << GetCeil("time")
              << GetCeil("repeat") << std::endl;
    //std::cout <<  "R_num "<< "\t" << "W_num" << "\t" <<  "N" << "\t\t" << "result" << "\t" << "time" <<  "\t\t\t" << "repeat" <<std::endl;
}

void PrintData(TestData data) {

    std::cout << GetCeil(data.R_num) << GetCeil(data.W_num) << GetCeil(data.N) << GetCeil(data.result)
              << GetCeil(data.time) << GetCeil(data.repeat) << std::endl;
    // std::cout <<  data.R_num << "\t\t" << data.W_num << "\t\t" <<  data.N << "\t\t" << data.result << "\t" << data.time <<  "\t\t" << data.repeat <<std::endl;
}



void PrintTitle(int k) {
    if (k == 0) {
        std::cout << "-------------\t\tBlocked Stack    \t\t-------------" << std::endl;
    } else {
        std::cout << "-------------\t\tNon Blocked Stack\t\t-------------" << std::endl;
    }
}

#endif //LAB_3_STACK__TESTDATA_H
