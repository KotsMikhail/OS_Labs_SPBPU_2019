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
WriteTest<T>::WriteTest(Set<T>* set, int writers_num, int records_num, std::string name) :
  Test<T>(name), curr_set(set), writers_num(writers_num), records_num(records_num)
{
  for (int i = 0; i < writers_num; i++)
  {
    data_set<T> data;
    for (int j = 0; j < records_num; j++)
    {
      data.push_back(static_cast<T>(i * records_num + j));
    }
    data_sets.push_back(data);
  }
}

template<typename T>
void WriteTest<T>::run() const
{
  std::vector<pthread_t> thread_ids;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  for (int i = 0; i < writers_num; i++)
  {
    auto info = new TestInfo<T>(curr_set, data_sets[i]);
    pthread_t tid;
    // TODO: handle error
    pthread_create(&tid, &attr, threadFunc, info);
    thread_ids.push_back(tid);
  }
  for (auto id : thread_ids)
  {
    pthread_join(id, nullptr);
  }
}

template<typename T>
void WriteTest<T>::check() const
{
  for (const auto& data_set : data_sets)
  {
    for (int value : data_set)
    {
      if (!curr_set->contains(value))
      {
        std::string msg = "Value not found: " + std::to_string(value);
        throw TestException(msg);
      }
    }
  }
}

template<typename T>
WriteTest<T>::~WriteTest()
{
  delete curr_set;
}
