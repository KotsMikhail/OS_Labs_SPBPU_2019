#include <iostream>
#include <cassert>
#include <vector>
#include <sstream>
#include "optimistic_list/optimistic_list.h"

using data_set = std::vector<int>;

struct TestInfo
{
  Set<int>& testing_set;
  data_set data;

  TestInfo(Set<int>& set, data_set data): testing_set(set), data(std::move(data))
  {}
};

template<typename T>
std::string to_str(T begin, T end)
{
  std::stringstream ss;
  bool first = true;
  for (; begin != end; begin++)
  {
    if (!first)
    {
      ss << ", ";
    }
    ss << *begin;
    first = false;
  }
  return ss.str();
}

void* write(void *param)
{
  std::string tag = std::to_string(pthread_self());
  Logger::logDebug(tag, "write()");
  auto info = reinterpret_cast<TestInfo*>(param);
  Logger::logDebug(tag, "\n\tTestInfo.data = " + to_str(info->data.begin(), info->data.end()));
  for (int value : info->data)
  {
    Logger::logDebug(std::to_string(pthread_self()), "writing: " + std::to_string(value));
    info->testing_set.add(value);
  }
  delete info;
  pthread_exit(nullptr);
}

void checkAllWritten(Set<int>& testing_set, std::vector<data_set>& data_sets)
{
  for (const auto& data_set : data_sets)
  {
    for (int value : data_set)
    {
      assert(testing_set.contains(value) || ("expected value = " + std::to_string(value)).c_str());
    }
  }
}

int main()
{
  Logger::init();
  OptimisticList<int> ol;
  std::vector<pthread_t> thread_ids;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  std::vector<data_set> data_sets = {
    {0, 1, 2, 3, 4, 5},
    {6, 7, 8},
    {9},
    {10, 11, 12},
    {13, 14, 15, 16, 17}
  };
  int THREAD_NUM = data_sets.size();
  for (int i = 0; i < THREAD_NUM; i++)
  {
    TestInfo* info = new TestInfo(ol, data_sets[i]);
    pthread_t tid;
    pthread_create(&tid, &attr, write, info);
    thread_ids.push_back(tid);
  }
  for (auto id : thread_ids)
  {
    pthread_join(id, nullptr);
  }
  checkAllWritten(ol, data_sets);
  std::cout << "Basic OptimisticList test for writing done!" << std::endl;
  return 0;
}