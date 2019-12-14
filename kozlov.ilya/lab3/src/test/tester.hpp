#include <vector>
#include <logger/logger.h>
#include <sstream>
#include "../optimistic_list/optimistic_list.h"
#include "write_test/write_test.h"
#include "../lazy_list/lazy_list.h"
#include "read_test/read_test.h"
#include "general_test/general_test.h"

template<typename T>
Tester<T>::Tester() : writers_num(default_num), records_num(default_num),
  readers_num(default_num), reads_num(default_num)
{
}

template<typename T>
bool Tester<T>::LoadConfig(std::string file_name)
{
  // TODO: load config
  // TODO: handle error
  // TODO: general test
  tests.push_back(new WriteTest<T>(new OptimisticList<T>(), writers_num, records_num, "OptimisticList writing test"));
  tests.push_back(new ReadTest<T>(new OptimisticList<T>(), readers_num, reads_num, "OptimisticList reading test"));
  tests.push_back(new GeneralTest<T>(new OptimisticList<T>(), writers_num, records_num,
                                     readers_num, reads_num, "OptimisticList general test"));
  tests.push_back(new WriteTest<T>(new LazyList<T>(), writers_num, records_num, "LazyList writing test"));
  tests.push_back(new ReadTest<T>(new LazyList<T>(), readers_num, reads_num, "LazyList reading test"));
  tests.push_back(new GeneralTest<T>(new LazyList<T>(), writers_num, records_num,
                                     readers_num, reads_num, "LazyList general test"));
  return true;
}

template<typename T>
void Tester<T>::runTests() const
{
  Util::Modifier red(Util::Color::RED);
  Util::Modifier green(Util::Color::GREEN);
  Util::Modifier blue(Util::Color::BLUE);
  Util::Modifier def(Util::Color::DEFAULT);
  for (auto test: tests)
  {
    std::cout << blue << test->name << "..." << def << std::endl;
    test->run();
    try
    {
      test->check();
      std::cout << green << "\tSUCCEEDED!" << def << std::endl;
    }
    catch (TestException& te)
    {
      std::cout << red << " \tFAILED!" << std::endl << def << "\tReason: " << te.what() << std::endl;
    }
  }
}

template<typename T>
Tester<T>::~Tester()
{
  for (auto it = tests.begin(); it != tests.end(); it++)
  {
    delete *it;
  }
  tests.clear();
}
