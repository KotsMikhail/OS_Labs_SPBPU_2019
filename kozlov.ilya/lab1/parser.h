//
// Created by ilya on 10/10/19.
//

#ifndef LAB1__PARSER_H_
#define LAB1__PARSER_H_

#include <string>
#include <map>
#include <fstream>

class Parser
{
public:
  enum ConfigName
  {
    INTERVAL,
    DIR1,
    DIR2,
    ERROR
  };

  static std::map<ConfigName, std::string> ParseFile(std::ifstream& in_file);
private:
  static const std::string DELIM;
  static const std::map<std::string, ConfigName> config_map;
};

#endif //LAB1__PARSER_H_
