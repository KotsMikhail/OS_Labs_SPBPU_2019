//
// Created by peter on 10/25/19.
//

#ifndef LAB1_CONFIG_PARSER_H
#define LAB1_CONFIG_PARSER_H

#include <map>
#include <unordered_set>

class ConfigParser
{
public:
  using sset = std::unordered_set<std::string>;

  static ConfigParser &getInstance();
  std::map<std::string, std::string> parse( std::ifstream &input, const sset &parameterNames );
};

#endif //LAB1_CONFIG_PARSER_H

// END OF 'config_parser.h' FILE
