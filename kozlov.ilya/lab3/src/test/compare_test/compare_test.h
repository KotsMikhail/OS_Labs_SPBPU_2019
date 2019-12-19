#ifndef TEST_COMPARE_TEST_H
#define TEST_COMPARE_TEST_H

#include <vector>
#include <string>
#include <cmath>
#include <map>
#include "../test.h"

template<typename T>
class TestCreator;

template<typename T>
class CompareTest: public Test<T>
{
private:
  friend TestCreator<T>;

  enum class DataType
  {
    RANDOM,
    FIXED
  };

  int writers_num;
  int records_num;
  int readers_num;
  int reads_num;
  int times;
  size_t succeeded_count;
  std::vector<std::string> names;
  std::vector<Test<T>*> tests;
  std::map<std::string, double> test_results;
  const std::string tag = "CompareTest";

  data_set<T> createRandom();
  data_set<T> createFixed();
  static std::string toString(const Type& set_type, const TestType& test_type, const DataType& type);
  void createSetOfTests(const TestCreator<T>& creator, const Type& set_type,
    const TestType& test_type, const DataType& type, const data_set<T>& data);

  CompareTest(int times, const Type& set_type1, const Type& set_type2, int writers_num, int records_num,
              int readers_num, int reads_num, const std::string& name) noexcept;
public:
  void run() override;
  void check() const override;
  ~CompareTest() override;
};

#include "compare_test.hpp"

#endif // TEST_COMPARE_TEST_H
