//
// Created by ilya on 10/10/19.
//

#include "parser.h"

#include <vector>

std::string const Parser::DELIM = " :=";

std::map<std::string, Parser::ConfigName> const Parser::config_map = {
    {"interval", INTERVAL},
    {"dir1", DIR1},
    {"dir2", DIR2},
    {"error", ERROR}
};

std::map<Parser::ConfigName, std::string> Parser::ParseFile(std::ifstream& in_file)
{
  std::map<ConfigName, std::string> res_dict;
  std::map<ConfigName, std::string> err_map;
  std::string cur_str;
  while (!in_file.eof())
  {
    std::getline(in_file, cur_str);
    std::vector<std::string> words;
    std::string cur_word;
    for (char ch : cur_str)
    {
      if (isspace(ch) || DELIM.find(ch) != std::string::npos)
      {
        if (!cur_word.empty())
        {
          words.push_back(cur_word);
        }
        cur_word = "";
        continue;
      }
      cur_word.push_back(ch);
    }
    if (!cur_word.empty())
    {
      words.push_back(cur_word);
    }
    unsigned long cur_words_size = words.size();
    if (cur_words_size == 0)
    {
      continue;
    }
    if (cur_words_size != 2)
    {
      std::string words_str;
      for (std::string& w : words)
      {
        words_str += w;
      }
      // Error map with one pair = [ERROR, <error value>]
      std::string error = "Wrong words number = " + std::to_string(cur_words_size);
      err_map.insert(std::pair<ConfigName, std::string>(ERROR, error));
      return err_map;
    }
    if (config_map.find(words[0]) != config_map.end())
    {
      res_dict.insert(std::pair<ConfigName, std::string>(config_map.at(words[0]), words[1]));
    }
    else
    {
      // Error map with one pair = [ERROR, <error value>]
      std::string error = "Wrong config name = " + words[0];
      err_map.insert(std::pair<ConfigName, std::string>(ERROR, error));
      return err_map;
    }
  }
  return res_dict;
}