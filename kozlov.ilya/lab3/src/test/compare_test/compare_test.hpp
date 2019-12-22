#include <logger/logger.h>
#include <util.h>
#include <cassert>
#include <random>
#include <algorithm>
#include <iomanip>
#include <timer.h>
#include "../test_creator/test_creator.h"

template<typename T>
std::string CompareTest<T>::toString(const Type& set_type, const TestType& test_type, const DataType& type)
{
  std::string res;
  switch (set_type)
  {
    case Type::OPTIMISTIC:
      res += "Optimistic ";
      break;
    case Type::LAZY:
      res += "Lazy ";
      break;
  }
  switch (test_type)
  {
    case TestType::WRITE:
      res += "writing ";
      break;
    case TestType::READ:
      res += "reading ";
      break;
    case TestType::GENERAL:
      res += "general ";
      break;
  }
  switch (type)
  {
    case DataType::RANDOM:
      res += "(random)";
      break;
    case DataType::FIXED:
      res += "(fixed)";
      break;
  }
  return res;
}

template<typename T>
CompareTest<T>::CompareTest(int times, const Type& set_type1, const Type& set_type2, int writers_num, int records_num,
                            int readers_num, int reads_num, const std::string& name) noexcept:
  Test<T>(name), writers_num(writers_num), records_num(records_num),
  readers_num(readers_num), reads_num(reads_num), times(times), succeeded_count(0)
{
  Logger::logDebug(tag, "constructing...");
  auto creator = TestCreator<T>(writers_num, records_num, readers_num, reads_num, times);
  auto random_data = createRandom();
  auto fixed_data = createFixed();
  createSetOfTests(creator, set_type1, TestType::WRITE, DataType::RANDOM, random_data);
  createSetOfTests(creator, set_type2, TestType::WRITE, DataType::RANDOM, random_data);
  createSetOfTests(creator, set_type1, TestType::WRITE, DataType::FIXED, fixed_data);
  createSetOfTests(creator, set_type2, TestType::WRITE, DataType::FIXED, fixed_data);
  createSetOfTests(creator, set_type1, TestType::READ, DataType::RANDOM, random_data);
  createSetOfTests(creator, set_type2, TestType::READ, DataType::RANDOM, random_data);
  createSetOfTests(creator, set_type1, TestType::READ, DataType::FIXED, fixed_data);
  createSetOfTests(creator, set_type2, TestType::READ, DataType::FIXED, fixed_data);
  createSetOfTests(creator, set_type1, TestType::GENERAL, DataType::RANDOM, random_data);
  createSetOfTests(creator, set_type2, TestType::GENERAL, DataType::RANDOM, random_data);
  createSetOfTests(creator, set_type1, TestType::GENERAL, DataType::FIXED, fixed_data);
  createSetOfTests(creator, set_type2, TestType::GENERAL, DataType::FIXED, fixed_data);
}

template<typename T>
void CompareTest<T>::createSetOfTests(const TestCreator<T>& creator, const Type& set_type,
  const TestType& test_type, const CompareTest::DataType& type, const data_set<T>& data)
{
  std::string test_name = toString(set_type, test_type, type);
  names.push_back(test_name);
  for (int i = 0; i < times; i++)
  {
    tests.push_back(creator.get(set_type, test_type, test_name, data));
  }
}

template<typename T>
data_set<T> CompareTest<T>::createRandom()
{
  data_set<T> data;
  for (int i = 0; i < readers_num; i++)
  {
    for (int j = 0; j < reads_num; j++)
    {
      data.push_back(static_cast<T>(i * reads_num + j));
    }
  }
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(data.begin(), data.end(), g);
  return data;
}

template<typename T>
data_set<T> CompareTest<T>::createFixed()
{
  data_set<T> data;
  for (int i = 0; i < readers_num; i++)
  {
    for (int j = 0; j < reads_num; j++)
    {
      data.push_back(static_cast<T>(i + j * readers_num));
    }
  }
  return data;
}

template<typename T>
void CompareTest<T>::run()
{
  Timer timer;
  for (size_t i = 0; i < tests.size(); i++)
  {
    if (tests[i])
    {
      tests[i]->run();
      try
      {
        tests[i]->check();
        succeeded_count += 1;
        if ((i + 1) % times == 0)
        {
          double time = timer.elapsed();
          test_results[tests[i]->name] = time / times;
          timer.reset();
        }
      }
      catch (TestException& te)
      {
        std::cout << " \tFAILED!" << std::endl << "\tReason: " << te.what() << std::endl;
        break;
      }
    }
  }
  int l_w = 30;
  int r_w = 10;
  std::cout << std::left << std::setw(l_w) << "NAME" << " | "  << std::setw(r_w) << "TIME" << std::endl;
  for (size_t i = 0; i < names.size(); i++)
  {
    if (i % 2 == 0)
    {
      std::cout << std::setfill('-') << std::setw(l_w + 2 * r_w) << "" << std::setfill(' ') << std::endl;
    }
    std::cout << std::setw(l_w) << names[i] << " | " << std::setw(r_w) << test_results[names[i]] << std::endl;
  }
}

template<typename T>
void CompareTest<T>::check() const
{
  if (succeeded_count != tests.size())
  {
    std::string msg = "Some of tests failed";
    throw TestException(msg);
  }
}

template<typename T>
CompareTest<T>::~CompareTest()
{
  Logger::logDebug(tag, "destructing");
  for (auto it = tests.begin(); it != tests.end(); it++)
  {
    delete *it;
  }
  tests.clear();
}
