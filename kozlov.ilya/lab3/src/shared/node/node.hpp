
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
  struct timespec cur_time{};
  struct timespec start_time{};
  clock_gettime(CLOCK_REALTIME, &start_time);
  while (true)
  {
    int code = pthread_mutex_trylock(&mutex);
    if (code == EBUSY)
    {
      clock_gettime(CLOCK_REALTIME, &cur_time);
      if (cur_time.tv_sec - start_time.tv_sec > timeout)
      {
        pthread_yield();
        start_time = cur_time;
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
