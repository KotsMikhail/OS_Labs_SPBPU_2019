//
// Created by peter on 10/25/19.
//

#include <vector>
#include <fstream>
#include <algorithm>

#include "config_parser.h"

ConfigParser &ConfigParser::getInstance()
{
  static ConfigParser instance;

  return instance;
} // end of 'ConfigParser::getInstance' function

std::map<std::string, std::string> ConfigParser::parse( std::ifstream &input,
                                                        const sset &parameterNames )
{
  std::map<std::string, std::string> output;
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
      throw std::runtime_error("ERROR: line " + std::to_string(line_num) + ": '" + line +
                               "' wrong number of words!");

    // check if parameter name is acceptable and save
    if(std::find(parameterNames.begin(), parameterNames.end(), words[0]) != parameterNames.end())
      output.insert(std::pair<std::string, std::string>(words[0], words[1]));
    else
      throw std::runtime_error("ERROR: line " + std::to_string(line_num) + ": '" + line +
                               "' unknown parameter name!");
  }

  return output;
} // end of 'ConfigParser::parse' function

// END OF 'config_parser.cpp' FILE
