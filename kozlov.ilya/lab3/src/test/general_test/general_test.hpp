#include <logger/logger.h>
#include <util.h>
#include <cassert>
#include "general_test.h"

template<typename T>
std::vector<int> GeneralTest<T>::check_array;

template<typename T>
void* GeneralTest<T>::threadWriteFunc(void *param)
{
  std::string tag = std::to_string(pthread_self());
  Logger::logDebug(tag, "write()");
  auto info = reinterpret_cast<TestInfo<T>*>(param);
  Logger::logDebug(tag, "\n\tTestInfo.data = " + Util::toStr(info->data.begin(), info->data.end()));
  for (int value : info->data)
  {
    // TODO: handle error
    Logger::logDebug(std::to_string(pthread_self()), "writing: " + std::to_string(value));
    info->testing_set->add(value);
  }
  delete info;
  pthread_exit(nullptr);
}


template<typename T>
void* GeneralTest<T>::threadReadFunc(void *param)
{
  std::string tag = std::to_string(pthread_self());
  Logger::logDebug(tag, "read()");
  auto info = reinterpret_cast<TestInfo<T>*>(param);
  Logger::logDebug(tag, "\n\tTestInfo.data = " + Util::toStr(info->data.begin(), info->data.end()));
  for (int value : info->data)
  {
    // TODO: handle error
    Logger::logDebug(std::to_string(pthread_self()), "reading: " + std::to_string(value));
    info->testing_set->remove(value);
    check_array[(int)value]++;
  }
  delete info;
  pthread_exit(nullptr);
}

template<typename T>
GeneralTest<T>::GeneralTest(Set<T>* set, int writers_num, int records_num,
  int readers_num, int reads_num, std::string name):
  Test<T>(name), curr_set(set), writers_num(writers_num), records_num(records_num),
  readers_num(readers_num), reads_num(reads_num)
{
    /*
  std::string msg = "writers_num = " + std::to_string(writers_num) + "\nrecords_num = " + std::to_string(records_num) +
    "\nreaders_num = " + std::to_string(readers_num) + "\nreads_num = " + std::to_string(reads_num) + "\n";
  Logger::logNote("GeneralTest", msg);
     */
  for (int i = 0; i < readers_num; i++)
  {
    for (int j = 0; j < reads_num; j++)
    {
      T element = static_cast<T>(i * reads_num + j);
      data_sets.push_back(element);
      check_array.push_back(0);
    }
  }
}

template<typename T>
void GeneralTest<T>::run() const
{
  check_array.clear();
  std::vector<pthread_t> writers_thread_ids;
  std::vector<pthread_t> readers_thread_ids;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  for (int i = 0; i < writers_num; i++)
  {
    std::vector<T> data = std::vector<T>(data_sets.begin() + i * records_num,
      data_sets.begin() + (i + 1) * records_num);
    auto info = new TestInfo<T>(curr_set, data);
    pthread_t tid;
    // TODO: handle error
    pthread_create(&tid, &attr, threadWriteFunc, info);
    writers_thread_ids.push_back(tid);
  }
  for (auto id : writers_thread_ids)
  {
    pthread_join(id, nullptr);
  }
  for (int i = 0; i < readers_num; i++)
  {
    std::vector<T> data = std::vector<T>(data_sets.begin() + i * reads_num,
                                      data_sets.begin() + (i + 1) * reads_num);
    auto info = new TestInfo<T>(curr_set, data);
    pthread_t tid;
    // TODO: handle error
    pthread_create(&tid, &attr, threadReadFunc, info);
    readers_thread_ids.push_back(tid);
  }
  for (auto id : readers_thread_ids)
  {
    pthread_join(id, nullptr);
  }
}

template<typename T>
void GeneralTest<T>::check() const
{
  if (!curr_set->empty())
  {
    std::string msg = "Set is not empty";
    throw TestException(msg);
  }
  for (size_t i = 0; i < check_array.size(); i++)
  {
    if (check_array[i] != 1)
    {
      std::string msg = "Element with index = " + std::to_string(i) + " was not read";
      throw TestException(msg);
    }
  }
}

template<typename T>
GeneralTest<T>::~GeneralTest()
{
  delete curr_set;
}
