#ifndef WRITE_TEST_H
#define WRITE_TEST_H

#include <vector>
#include <string>
#include "../test.h"

template<typename T>
class WriteTest: public Test<T>
{
private:
  Set<T>* curr_set;
  int writers_num;
  int records_num;
  std::vector<data_set<T>> data_sets;
  const std::string tag = "WriteTest";

public:
  WriteTest(Set<T>* set, int writers_num, int records_num, std::string name) noexcept;
  void run() const override;
  void check() const override;
  ~WriteTest() override;

private:
  static void* threadFunc(void *param);
};

#include "write_test.hpp"

#endif // WRITE_TEST_H
