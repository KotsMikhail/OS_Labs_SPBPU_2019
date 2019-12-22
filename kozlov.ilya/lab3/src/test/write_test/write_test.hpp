#include <string>
#include <logger/logger.h>
#include <util.h>
#include <cassert>

template<typename T>
void* WriteTest<T>::threadFunc(void *param)
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
WriteTest<T>::WriteTest(Set<T>* set, int writers_num, int records_num, const std::string& name) noexcept:
  Test<T>(name), curr_set(set), writers_num(writers_num), records_num(records_num)
{
  Logger::logDebug(tag, "constructing...");
  for (int i = writers_num - 1; i >= 0; i--)
  {
    for (int j = records_num - 1; j >= 0; j--)
    {
      T element = static_cast<T>(i * records_num + j);
      data_sets.push_back(element);
    }
  }
}

template<typename T>
WriteTest<T>::WriteTest(const data_set<T>& data_sets, Set<T>* set,
  int writers_num, int records_num, const std::string& name) noexcept:
  Test<T>(name), curr_set(set), writers_num(writers_num), records_num(records_num), data_sets(data_sets)
{
  Logger::logDebug(tag, "constructing...");
}

template<typename T>
void WriteTest<T>::run()
{
  Logger::logDebug(tag, "run");
  std::vector<pthread_t> thread_ids;
  pthread_attr_t attr;
  if (pthread_attr_init(&attr) != 0)
  {
    Logger::logError(tag, "Cant init pthread attributes");
    return;
  }
  for (int i = 0; i < writers_num; i++)
  {
    std::vector<T> data = std::vector<T>(data_sets.begin() + i * records_num,
                                         data_sets.begin() + (i + 1) * records_num);
    auto info = new TestInfo<T>(curr_set, data);
    pthread_t tid;
    if (pthread_create(&tid, &attr, threadFunc, info) != 0)
    {
      Logger::logError(tag, "Cant create pthread");
      return;
    }
    thread_ids.push_back(tid);
  }
  for (auto id : thread_ids)
  {
    if (pthread_join(id, nullptr) != 0)
    {
      Logger::logError(tag, "Cant join pthread with id == " + std::to_string(id) + "\n");
      return;
    }
  }
}

template<typename T>
void WriteTest<T>::check() const
{
  Logger::logDebug(tag, "check");
  for (T value : data_sets)
  {
    if (!curr_set->contains(value))
    {
      std::string msg = "Value not found: " + std::to_string(value);
      throw TestException(msg);
    }
  }
}

template<typename T>
WriteTest<T>::~WriteTest()
{
  Logger::logDebug(tag, "destructing");
  delete curr_set;
}
