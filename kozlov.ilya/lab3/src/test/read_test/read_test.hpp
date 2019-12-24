#include <logger/logger.h>
#include <util.h>
#include <cassert>

template<typename T>
void* ReadTest<T>::threadFunc(void *param)
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
      if (!info->testing_set->remove(value))
      {
        Logger::logError(tag, "Cant read value = " + std::to_string(value) + "\n");
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
ReadTest<T>::ReadTest(Set<T>* set, int readers_num, int reads_num, const std::string& name) noexcept:
  Test<T>(name), curr_set(set), readers_num(readers_num), reads_num(reads_num)
{
  Logger::logDebug(tag, "constructing...");
  try
  {
    for (int i = readers_num - 1; i >= 0; i--)
    {
      for (int j = reads_num - 1; j >= 0; j--)
      {
        T element = static_cast<T>(i * reads_num + j);
        data_sets.push_back(element);
        set->add(element);
      }
    }
  }
  catch (std::exception& e)
  {
    Logger::logError(tag, e.what());
  }
}

template<typename T>
ReadTest<T>::ReadTest(const data_set<T>& data_sets, Set<T>* set, int readers_num, int reads_num, const std::string& name) noexcept:
  Test<T>(name), curr_set(set), readers_num(readers_num), reads_num(reads_num), data_sets(data_sets)
{
  try
  {
    for (int i = 0; i < readers_num; i++)
    {
      for (int j = 0; j < reads_num; j++)
      {
        set->add(data_sets[i * reads_num + j]);
      }
    }
  }
  catch (std::exception& e)
  {
    Logger::logError(tag, e.what());
  }
}

template<typename T>
void ReadTest<T>::run()
{
  Logger::logDebug(tag, "run");
  std::vector<pthread_t> thread_ids;
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
void ReadTest<T>::check() const
{
  Logger::logDebug(tag, "check");
  if (!curr_set->empty())
  {
    std::string msg = "Set is not empty";
    throw TestException(msg);
  }
}

template<typename T>
ReadTest<T>::~ReadTest()
{
  Logger::logDebug(tag, "destructing");
  delete curr_set;
}
