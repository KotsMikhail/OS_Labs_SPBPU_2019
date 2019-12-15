
#include <logger/logger.h>
#include <cstring>
#include <timer.h>

template<typename T>
Node<T>::Node(const T& item, int key, pthread_mutex_t& mutex) noexcept: item(item), key(key), mutex(mutex), next(nullptr)
{
}

template<typename T>
bool Node<T>::operator==(const Node& other) const
{
  return key == other.key && item == other.item;
}

template<typename T>
int Node<T>::timedLock()
{
  Timer t;
  while (true)
  {
    int code = pthread_mutex_trylock(&mutex);
    if (code == EBUSY)
    {
      if (t.elapsed() > timeout)
      {
        Logger::logDebug("Node", "timed out locking, yielding...");
        pthread_yield();
        t.reset();
      }
      continue;
    }
    else
    {
      return code;
    }
  }
}

template<typename T>
int Node<T>::unlock()
{
  return pthread_mutex_unlock(&mutex);
}

template<typename T>
Node<T>::~Node()
{
  unlock();
  pthread_mutex_destroy(&mutex);
}
