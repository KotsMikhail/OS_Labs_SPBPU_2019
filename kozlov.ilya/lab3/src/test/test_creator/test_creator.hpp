
#include "../write_test/write_test.h"

template<typename T>
TestCreator<T>::TestCreator(int writers_num, int records_num, int readers_num, int reads_num) noexcept:
  writers_num(writers_num), records_num(records_num), readers_num(readers_num), reads_num(reads_num)
{
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
    return new WriteTest<T>(set, writers_num, records_num, name);
  case TestType::READ:
    return new ReadTest<T>(set, readers_num, reads_num, name);
  case TestType::GENERAL:
    if (writers_num * records_num == readers_num * reads_num)
    {
      return new GeneralTest<T>(set, writers_num, records_num, readers_num, reads_num, name);
    }
    Logger::logError(tag, "Count of total read must be equal to total writes");
    return nullptr;
  case TestType::COMPARE:
    // TODO:
    return nullptr;
  }
  return nullptr;
}