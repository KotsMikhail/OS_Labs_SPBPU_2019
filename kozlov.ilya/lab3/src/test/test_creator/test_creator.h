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
  int times;
  int max_threads;
public:
  TestCreator(int writers_num, int records_num, int readers_num, int reads_num, int times) noexcept;
  Test<T>* get(const Type& set_type, const TestType& type, const std::string& name, const data_set<T>& data = {}) const;
  Test<T>* compare(const Type& set_type1, const Type& set_type2, const std::string& name) const;
};

#include "test_creator.hpp"

#endif // TEST_CREATOR_H
