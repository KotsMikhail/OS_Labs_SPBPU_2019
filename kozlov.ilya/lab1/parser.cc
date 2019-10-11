//
// Created by ilya on 10/10/19.
//

#include "parser.h"

#include <vector>

std::map<std::string, std::string> ParseFile(std::ifstream& in_file, const std::string& delim)
{
  std::map<std::string, std::string> res_dict;
  std::string cur_str;
  while (!in_file.eof())
  {
    std::getline(in_file, cur_str);
    std::vector<std::string> words;
    std::string cur_word;
    for (char ch : cur_str)
    {
      if (isspace(ch) || delim.find(ch) != std::string::npos)
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
    if (words.size() != 2)
    {
      std::string words_str;
      for (std::string& w : words)
      {
        words_str += w;
      }
      std::map<std::string, std::string> err_map;
      // Error map with one pair = ["ERROR", <current words>]
      err_map.insert(std::pair<std::string, std::string>(PARSER_ERROR, words_str.c_str()));
      return err_map;
    }
    res_dict.insert(std::pair<std::string, std::string>(words[0], words[1]));
  }
  return res_dict;
}