
template<typename T>
Node<T>::Node(T item): item(item), next(nullptr)
{
  key = std::hash<T>()(item);
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&mutex, &attr);
}

template<typename T>
bool Node<T>::operator==(const Node& other)
{
  return key == other.key && item == other.item;
}

template<typename T>
int Node<T>::lock()
{
  return pthread_mutex_lock(&mutex);
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
