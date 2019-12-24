#ifndef TESTER_H
#define TESTER_H

#include <string>
#include <set.h>
#include <vector>
#include "test.h"
#include "../optimistic_list/optimistic_list.h"
#include "../lazy_list/lazy_list.h"
#include "test_creator/test_creator.h"

template<typename T>
class Tester
{
private:
  const std::string tag = "Tester";
  std::vector<Test<T>*> tests;

  void setTests(TestCreator<T>* creator);

public:
  bool loadConfig(const std::string& file_name);
  void runTests() const;
  ~Tester();
};

#include "tester.hpp"

#endif // TESTER_H
