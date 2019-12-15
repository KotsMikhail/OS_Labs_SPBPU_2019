
template<typename T>
bool NodeCreator<T>::initData()
{
  hash = std::hash<T>()(item);
  pthread_mutexattr_t attr;
  if (pthread_mutexattr_init(&attr) != 0)
  {
    return false;
  }
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  return !pthread_mutex_init(&mutex, &attr);
}

template<typename T>
NodeCreator<T>::NodeCreator(const T& item): failed(false), item(item)
{
  failed = !initData();
  if (failed)
  {
    Logger::logError(tag, "Cant create mutex");
  }
}

template<typename T>
template<class NodeT>
NodeT* NodeCreator<T>::get()
{
  if (failed)
  {
    return nullptr;
  }
  return new NodeT(item, hash, mutex);
}


