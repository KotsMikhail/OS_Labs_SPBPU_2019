//
// Created by misha on 24.11.2019.
//

#ifndef LAB_2_SEVER__MY_FUNCTIONS_H
#define LAB_2_SEVER__MY_FUNCTIONS_H


#include <string>
#include <vector>
#include <sstream>
#include "../interfaces/message.h"


using namespace std;
std::vector<int>& split(const std::string &s, char delim, std::vector<int> &elems);
std::vector<int> split(const std::string &str, char delim);
std::vector<int> dateFromStr(std::string date);
bool CheckDate(std::string date);
bool MessFromDate(std::string date, Message &mes);
template <typename T>
std::string toString(T val)
{
    std::ostringstream oss;
    oss<< val;
    return oss.str();
}


#endif //LAB_2_SEVER__MY_FUNCTIONS_H
