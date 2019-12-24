#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <map>
#include <fstream>

struct ParserException: public std::exception {
private:
  std::string message;
public:
  explicit ParserException(std::string& message) noexcept: message(message)
  {}

  const char* what() const noexcept override
  {
    return message.c_str();
  }
};

class Parser
{
public:
  enum class ConfigName
  {
    WRITERS_NUM,
    RECORDS_NUM,
    READERS_NUM,
    READS_NUM,
    TIMES
  };

  static std::map<ConfigName, std::string> parse(std::ifstream& in_file);
private:
  static const std::string DELIM;
  static const std::map<std::string, ConfigName> config_map;
};

#endif // PARSER_H
