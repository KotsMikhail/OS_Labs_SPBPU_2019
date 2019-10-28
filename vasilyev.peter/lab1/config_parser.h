//
// Created by peter on 10/25/19.
//

#ifndef LAB1_CONFIG_PARSER_H
#define LAB1_CONFIG_PARSER_H

#include <map>
#include <unordered_set>

class ConfigParser
{
private:
  ConfigParser() = default;

public:
  ConfigParser( const ConfigParser &configParser ) = delete;
  ConfigParser &operator=( const ConfigParser &configParser ) = delete;

  using sset = std::unordered_set<std::string>;
  using ssmap = std::map<std::string, std::string>;

  static ssmap parse( std::ifstream &input, const sset &parameterNames );
};

#endif //LAB1_CONFIG_PARSER_H

// END OF 'config_parser.h' FILE
