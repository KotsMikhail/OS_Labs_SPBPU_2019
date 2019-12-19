#ifndef WRITE_TEST_H
#define WRITE_TEST_H

#include <vector>
#include <string>
#include "../test.h"

template<typename T>
class TestCreator;

template<typename T>
class WriteTest: public Test<T>
{
private:
  friend TestCreator<T>;

  Set<T>* curr_set;
  int writers_num;
  int records_num;
  data_set<T> data_sets;
  const std::string tag = "WriteTest";

public:
  void run() override;
  void check() const override;
  ~WriteTest() override;

private:
  WriteTest(Set<T>* set, int writers_num, int records_num, const std::string& name) noexcept;
  WriteTest(const data_set<T>& data_sets, Set<T>* set, int writers_num, int records_num, const std::string& name) noexcept;

  static void* threadFunc(void *param);
};

#include "write_test.hpp"

#endif // WRITE_TEST_H
