//
// Created by misha on 21.12.2019.
//

#include <string>
#include <iostream>
#include "TestData.h"

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