#include <sys/resource.h>
#include "../write_test/write_test.h"

template<typename T>
TestCreator<T>::TestCreator(int writers_num, int records_num, int readers_num, int reads_num) noexcept:
  writers_num(writers_num), records_num(records_num), readers_num(readers_num), reads_num(reads_num)
{
  struct rlimit lim;
  getrlimit(RLIMIT_NPROC, &lim);
  max_threads = lim.rlim_max;
}

template<typename T>
Test<T>* TestCreator<T>::get(const Type& set_type, const TestType& type, std::string name)
{
  Set<T>* set = SetCreator<T>::get(set_type);
  if (set == nullptr)
  {
    return nullptr;
  }
  switch (type)
  {
  case TestType::WRITE:
    if (writers_num <= max_threads)
    {
      return new WriteTest<T>(set, writers_num, records_num, name);
    }
    Logger::logError(tag, "Too much writers");
    return nullptr;
  case TestType::READ:
    if (readers_num <= max_threads)
    {
      return new ReadTest<T>(set, readers_num, reads_num, name);
    }
    Logger::logError(tag, "Too much writers");
    return nullptr;
  case TestType::GENERAL:
    if (writers_num * records_num == readers_num * reads_num &&
      writers_num + readers_num <= max_threads)
    {
      return new GeneralTest<T>(set, writers_num, records_num, readers_num, reads_num, name);
    }
    Logger::logError(tag, "Too much threads or not equal reads/writes");
    return nullptr;
  case TestType::COMPARE:
    // TODO:
    return nullptr;
  }
  return nullptr;
}