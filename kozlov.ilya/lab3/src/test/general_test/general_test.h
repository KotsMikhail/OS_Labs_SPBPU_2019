#ifndef GENERAL_TEST_H
#define GENERAL_TEST_H

#include <vector>
#include <string>
#include <atomic>
#include "../test.h"

template<typename T>
class TestCreator;

template<typename T>
class GeneralTest: public Test<T>
{
private:
  friend TestCreator<T>;

  Set<T>* curr_set;
  int writers_num;
  int records_num;
  int readers_num;
  int reads_num;
  data_set<T> data_sets;
  std::vector<Test<T>*> tests;
  const std::string tag = "GeneralTest";

public:
  void run() override;
  void check() const override;
  ~GeneralTest() override;

private:
  GeneralTest(Set<T>* set, int writers_num, int records_num,
              int readers_num, int reads_num, const std::string& name) noexcept;
  GeneralTest(const data_set<T>& data_sets, Set<T>* set, int writers_num, int records_num,
              int readers_num, int reads_num, const std::string& name) noexcept;

  static std::vector<int> check_array;
  static void* threadWriteFunc(void *param);
  static void* threadReadFunc(void *param);
};

#include "general_test.hpp"

#endif // GENERAL_TEST_H