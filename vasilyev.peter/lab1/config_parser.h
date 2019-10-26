//
// Created by peter on 10/25/19.
//

#ifndef LAB1_CONFIG_PARSER_H
#define LAB1_CONFIG_PARSER_H

#include <map>

class ConfigParser
{
public:
  enum struct Parameter
  {
    TIME_INTERVAL,
    DIR1_NAME,
    DIR2_NAME,
    ERROR
  };

private:
  static const std::map<std::string, Parameter> nameToParameter;

public:
  static bool parse( std::ifstream &input, std::map<Parameter, std::string> &output );
};

#endif //LAB1_CONFIG_PARSER_H

// END OF 'config_parser.h' FILE
