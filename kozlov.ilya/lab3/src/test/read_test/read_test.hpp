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
  for (int value : info->data)
  {
    // TODO: handle error
    Logger::logDebug(std::to_string(pthread_self()), "reading: " + std::to_string(value));
    info->testing_set->remove(value);
  }
  delete info;
  pthread_exit(nullptr);
}

template<typename T>
ReadTest<T>::ReadTest(Set<T>* set, int readers_num, int reads_num, std::string name) :
  Test<T>(name), curr_set(set), readers_num(readers_num), reads_num(reads_num)
{
  for (int i = 0; i < readers_num; i++)
  {
    data_set<T> data;
    for (int j = 0; j < reads_num; j++)
    {
      T element = static_cast<T>(i * reads_num + j);
      data.push_back(element);
      set->add(element);
    }
    data_sets.push_back(data);
  }
}

template<typename T>
void ReadTest<T>::run() const
{
  std::vector<pthread_t> thread_ids;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  for (int i = 0; i < readers_num; i++)
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
void ReadTest<T>::check() const
{
  if (!curr_set->empty())
  {
    std::string msg = "Set is not empty";
    throw TestException(msg);
  }
}

template<typename T>
ReadTest<T>::~ReadTest()
{
  delete curr_set;
}
