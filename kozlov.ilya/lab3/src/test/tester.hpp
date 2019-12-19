#include <vector>
#include <logger/logger.h>
#include <sstream>
#include <fstream>
#include <timer.h>
#include "../optimistic_list/optimistic_list.h"
#include "write_test/write_test.h"
#include "../lazy_list/lazy_list.h"
#include "read_test/read_test.h"
#include "general_test/general_test.h"
#include "test_creator/test_creator.h"
#include "../parser/parser.h"

template<typename T>
bool Tester<T>::loadConfig(const std::string& file_name)
{
  std::ifstream in_file(file_name);
  if (!in_file)
  {
    Logger::logError(tag, "Cant open config file");
    return false;
  }
  try
  {
    auto dict = Parser::parse(in_file);
    int wr_n = std::stoi(dict.at(Parser::ConfigName::WRITERS_NUM));
    int rec_n = std::stoi(dict.at(Parser::ConfigName::RECORDS_NUM));
    int rd_n = std::stoi(dict.at(Parser::ConfigName::READERS_NUM));
    int reads_n = std::stoi(dict.at(Parser::ConfigName::READS_NUM));
    int times = std::stoi(dict.at(Parser::ConfigName::TIMES));
    if (wr_n > 0 && rec_n > 0 && rd_n > 0 && reads_n > 0)
    {
      auto creator = TestCreator<T>(wr_n, rec_n, rd_n, reads_n, times);
      setTests(&creator);
      return true;
    }
  }
  catch (ParserException& pe)
  {
    Logger::logError(tag, pe.what());
  }
  catch (std::exception& e)
  {
    Logger::logError(tag, e.what());
  }
  return false;
}

template<typename T>
void Tester<T>::setTests(TestCreator<T>* creator)
{
  tests.push_back(creator->get(Type::OPTIMISTIC, TestType::WRITE, "OptimisticList writing test"));
  tests.push_back(creator->get(Type::OPTIMISTIC, TestType::READ, "OptimisticList reading test"));
  tests.push_back(creator->get(Type::OPTIMISTIC, TestType::GENERAL, "OptimisticList general test"));
  tests.push_back(creator->get(Type::LAZY, TestType::WRITE, "LazyList writing test"));
  tests.push_back(creator->get(Type::LAZY, TestType::READ, "LazyList reading test"));
  tests.push_back(creator->get(Type::LAZY, TestType::GENERAL, "LazyList general test"));
  tests.push_back(creator->compare(Type::LAZY, Type::OPTIMISTIC, "Lazy vs Optimistic compare test"));
}

template<typename T>
void Tester<T>::runTests() const
{
  Util::Modifier red(Util::Color::RED);
  Util::Modifier green(Util::Color::GREEN);
  Util::Modifier blue(Util::Color::BLUE);
  Util::Modifier def(Util::Color::DEFAULT);
  int count = 0;
  double time_count = 0;
  Timer timer;
  for (auto test: tests)
  {
    if (test)
    {
      std::cout << blue << test->name << "..." << def << std::endl;
      timer.reset();
      test->run();
      try
      {
        test->check();
        std::cout << green << "\tSUCCEEDED!" << def << std::endl;
        count++;
      }
      catch (TestException& te)
      {
        std::cout << red << " \tFAILED!" << std::endl << def << "\tReason: " << te.what() << std::endl;
      }
      double time = timer.elapsed();
      time_count += time;
      std::cout << blue << "\tTIME in sec: " << time << def << std::endl;
    }
  }
  std::cout << blue << "TOTAL: " << green << count << " / " << tests.size() << blue << " PASSED" << def << std::endl;
  std::cout << blue << "\tTIME in sec: " << green << time_count << def << std::endl;
}

template<typename T>
Tester<T>::~Tester()
{
  for (auto it = tests.begin(); it != tests.end(); it++)
  {
    delete *it;
  }
  tests.clear();
}
