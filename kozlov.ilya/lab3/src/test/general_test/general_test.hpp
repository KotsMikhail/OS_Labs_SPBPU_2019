#include <logger/logger.h>
#include <util.h>
#include <cassert>
#include <timer.h>

template<typename T>
std::vector<int> GeneralTest<T>::check_array;

template<typename T>
void* GeneralTest<T>::threadWriteFunc(void *param)
{
  std::string tag = std::to_string(pthread_self());
  Logger::logDebug(tag, "write()");
  auto info = reinterpret_cast<TestInfo<T>*>(param);
  Logger::logDebug(tag, "\n\tTestInfo.data = " + Util::toStr(info->data.begin(), info->data.end()));
  try
  {
    for (int value : info->data)
    {
      Logger::logDebug(tag, "writing: " + std::to_string(value));
      if (!info->testing_set->add(value))
      {
        Logger::logError(tag, "Cant write to set value = " + std::to_string(value) + "\n");
        break;
      }
    }
  }
  catch (std::exception& e)
  {
    Logger::logError(tag, e.what());
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
  try
  {
    for (int value : info->data)
    {
      Logger::logDebug(tag, "reading: " + std::to_string(value));
      Timer t;
      while (!info->testing_set->remove(value))
      {
        Logger::logDebug(tag, "Yet cant read value = " + std::to_string(value) + "\n");
        if (t.elapsed() > t.TIMEOUT)
        {
          pthread_yield();
          t.reset();
        }
      }
      check_array[(int) value]++;
    }
  }
  catch (std::exception& e)
  {
    Logger::logError(tag, e.what());
  }
  delete info;
  pthread_exit(nullptr);
}

template<typename T>
GeneralTest<T>::GeneralTest(Set<T>* set, int writers_num, int records_num,
  int readers_num, int reads_num, const std::string& name) noexcept:
  Test<T>(name), curr_set(set), writers_num(writers_num), records_num(records_num),
  readers_num(readers_num), reads_num(reads_num)
{
  Logger::logDebug(tag, "constructing...");
  for (int i = readers_num - 1; i >= 0; i--)
  {
    for (int j = reads_num - 1; j >= 0; j--)
    {
      T element = static_cast<T>(i * reads_num + j);
      data_sets.push_back(element);
      check_array.push_back(0);
    }
  }
}

template<typename T>
GeneralTest<T>::GeneralTest(const data_set<T>& data_sets, Set<T>* set, int writers_num, int records_num,
  int readers_num, int reads_num, const std::string& name) noexcept:
  Test<T>(name), curr_set(set), writers_num(writers_num), records_num(records_num),
  readers_num(readers_num), reads_num(reads_num), data_sets(data_sets)
{
  for (int i = readers_num - 1; i >= 0; i--)
  {
    for (int j = reads_num - 1; j >= 0; j--)
    {
      check_array.push_back(0);
    }
  }
}

template<typename T>
void GeneralTest<T>::run()
{
  Logger::logDebug(tag, "run");
  check_array.clear();
  std::vector<pthread_t> writers_thread_ids;
  std::vector<pthread_t> readers_thread_ids;
  pthread_attr_t attr;
  if (pthread_attr_init(&attr) != 0)
  {
    Logger::logError(tag, "Cant init pthread attributes");
    return;
  }
  for (int i = 0; i < readers_num; i++)
  {
    std::vector<T> data = std::vector<T>(data_sets.begin() + i * reads_num,
                                         data_sets.begin() + (i + 1) * reads_num);
    auto info = new TestInfo<T>(curr_set, data);
    pthread_t tid;
    if (pthread_create(&tid, &attr, threadReadFunc, info) != 0)
    {
      Logger::logError(tag, "Cant create pthread");
      return;
    }
    readers_thread_ids.push_back(tid);
  }
  for (int i = 0; i < writers_num; i++)
  {
    std::vector<T> data = std::vector<T>(data_sets.begin() + i * records_num,
      data_sets.begin() + (i + 1) * records_num);
    auto info = new TestInfo<T>(curr_set, data);
    pthread_t tid;
    if (pthread_create(&tid, &attr, threadWriteFunc, info) != 0)
    {
      Logger::logError(tag, "Cant create pthread");
      return;
    }
    writers_thread_ids.push_back(tid);
  }
  for (auto id : readers_thread_ids)
  {
    if (pthread_join(id, nullptr) != 0)
    {
      Logger::logError(tag, "Cant join pthread with id == " + std::to_string(id) + "\n");
      return;
    }
  }
  for (auto id : writers_thread_ids)
  {
    if (pthread_join(id, nullptr) != 0)
    {
      Logger::logError(tag, "Cant join pthread with id == " + std::to_string(id) + "\n");
      return;
    }
  }
}

template<typename T>
void GeneralTest<T>::check() const
{
  Logger::logDebug(tag, "check");
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
  Logger::logDebug(tag, "destructing");
  delete curr_set;
}
