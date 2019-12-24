#include <sys/resource.h>
#include "../compare_test/compare_test.h"

template<typename T>
TestCreator<T>::TestCreator(int writers_num, int records_num, int readers_num, int reads_num, int times) noexcept:
  writers_num(writers_num), records_num(records_num), readers_num(readers_num), reads_num(reads_num), times(times)
{
  struct rlimit lim;
  getrlimit(RLIMIT_NPROC, &lim);
  max_threads = lim.rlim_max;
}

template<typename T>
Test<T>* TestCreator<T>::get(const Type& set_type, const TestType& type, const std::string& name, const data_set<T>& data) const
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
      if (data.empty())
      {
        return new WriteTest<T>(set, writers_num, records_num, name);
      }
      else
      {
        return new WriteTest<T>(data, set, writers_num, records_num, name);
      }
    }
    Logger::logError(tag, "Too much writers");
    return nullptr;
  case TestType::READ:
    if (readers_num <= max_threads)
    {
      if (data.empty())
      {
        return new ReadTest<T>(set, readers_num, reads_num, name);
      }
      else
      {
        return new ReadTest<T>(data, set, readers_num, reads_num, name);
      }
    }
    Logger::logError(tag, "Too much writers");
    return nullptr;
  case TestType::GENERAL:
    if (writers_num * records_num == readers_num * reads_num &&
      writers_num + readers_num <= max_threads)
    {
      if (data.empty())
      {
        return new GeneralTest<T>(set, writers_num, records_num, readers_num, reads_num, name);
      }
      else
      {
        return new GeneralTest<T>(data, set, writers_num, records_num, readers_num, reads_num, name);
      }
    }
    Logger::logError(tag, "Too much threads or not equal reads/writes");
    return nullptr;
  }
  return nullptr;
}

template<typename T>
Test<T>* TestCreator<T>::compare(const Type& set_type1, const Type& set_type2, const std::string& name) const
{
  if (writers_num * records_num == readers_num * reads_num &&
    writers_num + readers_num <= max_threads)
  {
    return new CompareTest<T>(times, set_type1, set_type2, writers_num, records_num, readers_num, reads_num, name);
  }
  return nullptr;
}