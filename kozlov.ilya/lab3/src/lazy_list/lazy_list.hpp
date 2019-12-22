#include <logger/logger.h>
#include <min_max/min_value.h>
#include <min_max/max_value.h>

template<typename T>
typename LazyList<T>::LazyNode* LazyList<T>::LazyNode::create(const T& item)
{
  int hash = std::hash<T>()(item);
  pthread_mutexattr_t attr;
  pthread_mutex_t mutex;
  if (pthread_mutexattr_init(&attr) != 0)
  {
    return nullptr;
  }
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  if (pthread_mutex_init(&mutex, &attr) != 0)
  {
    return nullptr;
  }
  return new LazyNode(item, hash, mutex);
}

template<typename T>
LazyList<T>::LazyList(LazyNode* head, const Collector<LazyNode>& collector):
  head(head), collector(collector)
{
}

template<typename T>
LazyList<T>::~LazyList()
{
  Logger::logDebug(tag, "destructing...");
  LazyNode* curr = head;
  while (curr != nullptr)
  {
    LazyNode* prev = curr;
    curr = curr->next;
    delete prev;
  }
}

template<typename T>
bool LazyList<T>::add(T element)
{
  Logger::logDebug(tag, "add(" + std::to_string(element) + ")");
  int key = std::hash<T>()(element);
  bool res = false;
  bool need_return = false;
  while (true)
  {
    LazyNode* prev = head;
    LazyNode* curr = prev->next;
    while (curr->key < key)
    {
      prev = curr;
      curr = curr->next;
    }
    prev->lock();
    curr->lock();
    if (validate(prev, curr))
    {
      need_return = true;
      if (curr->key == key)
      {
        res = false;
      }
      else
      {
        auto node = LazyNode::create(element);
        if (node == nullptr)
        {
          res = false;
        }
        else
        {
          node->next = curr;
          prev->next = node;
          res = true;
        }
      }
    }
    prev->unlock();
    curr->unlock();
    if (need_return)
    {
      return res;
    }
  }
}

template<typename T>
bool LazyList<T>::remove(T element)
{
  Logger::logDebug(tag, "remove(" + std::to_string(element) + ")");
  int key = std::hash<T>()(element);
  bool res = false;
  bool need_return = false;
  while (true)
  {
    LazyNode* prev = head;
    LazyNode* curr = prev->next;
    while (curr->key < key)
    {
      prev = curr;
      curr = curr->next;
    }
    prev->lock();
    curr->lock();
    if (validate(prev, curr))
    {
      need_return = true;
      if (curr->key == key)
      {
        curr->marked = true;
        prev->next = curr->next;
        curr->unlock();
        collector.add(curr);
        res = true;
      }
      else
      {
        res = false;
      }
    }
    prev->unlock();
    if (!res)
    {
      curr->unlock();
    }
    if (need_return)
    {
      return res;
    }
  }
}

template<typename T>
bool LazyList<T>::contains(T element) const
{
  Logger::logDebug(tag, "contains(" + std::to_string(element) + ")");
  int key = std::hash<T>()(element);
  LazyNode* curr = head;
  while (curr->key < key)
  {
    curr = curr->next;
  }
  return curr->key == key && !curr->marked;
}

template<typename T>
bool LazyList<T>::validate(LazyNode* prev, LazyNode* curr) const
{
  //Logger::logDebug(tag, "validating...");
  return !prev->marked && !curr->marked && prev->next == curr;
}

template<typename T>
bool LazyList<T>::empty() const
{
  return head->item == MinValue::get<T>() && head->next->item == MaxValue::get<T>();
}
