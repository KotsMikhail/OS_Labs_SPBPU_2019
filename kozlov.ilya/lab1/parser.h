//
// Created by ilya on 10/10/19.
//

#ifndef LAB1__PARSER_H_
#define LAB1__PARSER_H_

#include <string>
#include <map>
#include <fstream>

#define PARSER_ERROR "ERROR"

std::map<std::string, std::string> ParseFile(std::ifstream& in_file, const std::string& delim);

#endif //LAB1__PARSER_H_
