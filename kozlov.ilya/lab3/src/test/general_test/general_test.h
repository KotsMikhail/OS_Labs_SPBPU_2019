#ifndef GENERAL_TEST_H
#define GENERAL_TEST_H

#include <vector>
#include <string>
#include <atomic>
#include "../test.h"

template<typename T>
class GeneralTest: public Test<T>
{
private:
  Set<T>* curr_set;
  int writers_num;
  int records_num;
  int readers_num;
  int reads_num;
  std::vector<int> data_sets;
  const std::string tag = "GeneralTest";

public:
  GeneralTest(Set<T>* set, int writers_num, int records_num,
              int readers_num, int reads_num, std::string name) noexcept;
  void run() const override;
  void check() const override;
  ~GeneralTest() override;

private:
  static std::vector<int> check_array;
  static void* threadWriteFunc(void *param);
  static void* threadReadFunc(void *param);
};

#include "general_test.hpp"

#endif // GENERAL_TEST_H