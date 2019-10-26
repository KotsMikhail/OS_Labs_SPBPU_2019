//
// Created by peter on 10/25/19.
//

#include <vector>
#include <fstream>

#include "config_parser.h"

std::map<std::string, ConfigParser::Parameter> const ConfigParser::nameToParameter = {
  {"interval", Parameter::TIME_INTERVAL},
  {"dir1", Parameter::DIR1_NAME},
  {"dir2", Parameter::DIR2_NAME}
};

bool ConfigParser::parse( std::ifstream &input, std::map<Parameter, std::string> &output )
{
  int line_num = 0;
  std::string line;

  while (!input.eof())
  {
    std::getline(input, line);
    line_num++;

    std::vector<std::string> words;

    // separate line into words
    std::string word;
    for (char c : line)
    {
      // end of word
      if (isspace(c) || c == '=')
      {
        // save non-empty word
        if (word.length() != 0)
        {
          words.push_back(word);
          word.clear();
        }
        continue;
      }
      word.append(1, c);
    }
    // save last word
    if (word.length() != 0)
      words.push_back(word);

    // skip empty lines
    if (words.empty())
      continue;

    // parse line
    if (words.size() != 2)
    {
      std::string error = "ERROR: line " + std::to_string(line_num) + ": '" + line +
                          "' wrong number of words!";
      output.insert(std::pair<Parameter, std::string>(Parameter::ERROR, error));

      return false;
    }

    if (nameToParameter.find(words[0]) == nameToParameter.end())
    {
      std::string error = "ERROR: line " + std::to_string(line_num) + ": '" + line +
                          "' unknown parameter name!";
      output.insert(std::pair<Parameter , std::string>(Parameter::ERROR, error));

      return false;
    }
    else
    {
      std::pair<Parameter, std::string> value(nameToParameter.at(words[0]), words[1]);
      output.insert(value);
    }
  }

  return true;
} // end of 'ConfigParser::parse' function

// END OF 'config_parser.cpp' FILE
