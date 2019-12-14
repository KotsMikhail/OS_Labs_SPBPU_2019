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
  std::vector<data_set<T>> data_sets;
public:
  ReadTest(Set<T>* set, int readers_num, int reads_num, std::string name);
  void run() const override;
  void check() const override;
  ~ReadTest() override;
private:
  static void* threadFunc(void *param);
};

#include "read_test.hpp"

#endif // READ_TEST_H
