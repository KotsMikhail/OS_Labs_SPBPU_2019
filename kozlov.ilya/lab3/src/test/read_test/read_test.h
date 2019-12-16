#ifndef READ_TEST_H
#define READ_TEST_H

#include <vector>
#include <string>
#include "../test.h"

template<typename T>
class ReadTest: public Test<T>
{
private:
  Set<T>* curr_set;
  int readers_num;
  int reads_num;
  data_set<T> data_sets;
  const std::string tag = "ReadTest";

public:
  ReadTest(Set<T>* set, int readers_num, int reads_num, const std::string& name) noexcept;
  ReadTest(const data_set<T>& data_sets, Set<T>* set, int readers_num, int reads_num, const std::string& name) noexcept;
  void run() override;
  void check() const override;
  ~ReadTest() override;

private:
  static void* threadFunc(void *param);
};

#include "read_test.hpp"

#endif // READ_TEST_H
