//
// Created by misha on 24.11.2019.
//

#include "my_functions.h"
#include "../interfaces/message.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <syslog.h>

using namespace std;

std::vector<int>& split(const std::string &s, char delim, std::vector<int> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim))
    {
        elems.push_back(std::stoi(item));
    }
    return elems;
}


std::vector<int> split(const std::string &str, char delim)
{
    std::vector<int> elems;
    split(str, delim, elems);
    return elems;
}


std::vector<int> dateFromStr(std::string date) {
    std::vector<int> arr = split(date, '.');
    return arr;

}

bool CheckDate(std::string date) {
    std::vector<int> parsed_date = dateFromStr(date);
    if(parsed_date.size() < 3) {
        //std::cout << "ERROR: Wrong Date" << std::endl;
        syslog(LOG_ERR, "ERROR: Wrong Date");
        return false;
    }
    return true;
}

bool MessFromDate(std::string date, Message &mes) {
    std::vector<int> parsed_date = dateFromStr(date);
    if(parsed_date.size() < 3) {
        //std::cout << "ERROR: Wrong Date" << std::endl;
        syslog(LOG_ERR, "ERROR: Wrong Date");
        return false;
    }

    mes.day = parsed_date[0];
    mes.month = parsed_date[1];
    mes.year = parsed_date[2];

    return true;
}


