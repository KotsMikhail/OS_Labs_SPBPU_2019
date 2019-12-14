#ifndef TESTER_H
#define TESTER_H

#include <string>
#include <set.h>
#include <vector>
#include "test.h"
#include "../optimistic_list/optimistic_list.h"
#include "../lazy_list/lazy_list.h"

template<typename T>
class Tester
{
private:
  const int default_num = 5;
  int writers_num;
  int records_num;
  int readers_num;
  int reads_num;
  std::vector<Test<T>*> tests;
public:
  Tester();
  bool LoadConfig(std::string file_name);
  void runTests() const;
  ~Tester();
};

#include "tester.hpp"

#endif // TESTER_H
