#include "parser.h"

#include <vector>

std::string const Parser::DELIM = " :=";

std::map<std::string, Parser::ConfigName> const Parser::config_map =
{
  {"writers_num", ConfigName::WRITERS_NUM},
  {"records_num", ConfigName::RECORDS_NUM},
  {"readers_num", ConfigName::READERS_NUM},
  {"reads_num", ConfigName::READS_NUM},
  {"times", ConfigName::TIMES}
};

std::map<Parser::ConfigName, std::string> Parser::parse(std::ifstream& in_file)
{
  std::map<ConfigName, std::string> res_dict;
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
      std::string error = "Wrong words number = " + std::to_string(cur_words_size);
      throw ParserException(error);
    }
    if (config_map.find(words[0]) != config_map.end())
    {
      res_dict.insert(std::pair<ConfigName, std::string>(config_map.at(words[0]), words[1]));
    }
    else
    {
      std::string error = "Wrong config name = " + words[0];
      throw ParserException(error);
    }
  }
  return res_dict;
}