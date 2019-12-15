#ifndef TEST_CREATOR_H
#define TEST_CREATOR_H

#include <set_creator/set_creator.h>
#include "../test.h"
#include "../write_test/write_test.h"
#include "../read_test/read_test.h"
#include "../general_test/general_test.h"

template<typename T>
class TestCreator
{
private:
  const std::string tag = "TestCreator";
  int writers_num;
  int records_num;
  int readers_num;
  int reads_num;
public:
  TestCreator(int writers_num, int records_num, int readers_num, int reads_num) noexcept;
  Test<T>* get(const Type& set_type, const TestType& type, std::string name);
};

#include "test_creator.hpp"

#endif // TEST_CREATOR_H
