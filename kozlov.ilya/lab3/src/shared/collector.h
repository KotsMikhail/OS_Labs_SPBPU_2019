#ifndef NODE_COLLECTOR_H
#define NODE_COLLECTOR_H

#include <vector>
#include <pthread.h>
#include <stdexcept>

template<class T>
class Collector
{
private:
  std::vector<T*> deleted;
  pthread_mutex_t mutex;
public:
  int init()
  {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    return !pthread_mutex_init(&mutex, &attr);
  }

  void add(T* node)
  {
    bool res = false;
    if (pthread_mutex_lock(&mutex) == 0)
    {
      deleted.push_back(node);
      res = pthread_mutex_unlock(&mutex) == 0;
    }
    if (!res)
    {
      throw std::runtime_error("Mutex error while adding to collector");
    }
  }

  ~Collector()
  {
    for (auto it = deleted.begin(); it != deleted.end(); it++)
    {
      delete *it;
    }
    deleted.clear();
  }
};

#endif // NODE_COLLECTOR_H
