#include <logger/logger.h>
#include <min_max/min_value.h>
#include <min_max/max_value.h>

template<typename T>
LazyList<T>::LazyList()
{
  // TODO: handle error
  Logger::logDebug(tag, "constructing...");
  head = new LazyNode<T>(MinValue::get<T>());
  head->next = new LazyNode<T>(MaxValue::get<T>());
}

template<typename T>
LazyList<T>::~LazyList()
{
  Logger::logDebug(tag, "destructing...");
  LazyNode<T>* curr = head;
  while (curr != nullptr)
  {
    LazyNode<T>* prev = curr;
    curr = curr->next;
    delete prev;
  }
}

template<typename T>
bool LazyList<T>::add(T element)
{
  // TODO: try_lock
  Logger::logDebug(tag, "add(" + std::to_string(element) + ")");
  int key = std::hash<T>()(element);
  bool res = false;
  bool need_return = false;
  while (true)
  {
    LazyNode<T>* prev = head;
    LazyNode<T>* curr = prev->next;
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
        auto node = new LazyNode<T>(element);
        node->next = curr;
        prev->next = node;
        res = true;
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
  // TODO: try_lock
  Logger::logDebug(tag, "remove(" + std::to_string(element) + ")");
  int key = std::hash<T>()(element);
  bool res = false;
  bool need_return = false;
  while (true)
  {
    LazyNode<T>* prev = head;
    LazyNode<T>* curr = prev->next;
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
        delete curr;
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
  LazyNode<T>* curr = head;
  while (curr->key < key)
  {
    curr = curr->next;
  }
  return curr->key == key && !curr->marked;
}

template<typename T>
bool LazyList<T>::validate(LazyNode<T>* prev, LazyNode<T>* curr) const
{
  //Logger::logDebug(tag, "validating...");
  return !prev->marked && !curr->marked && prev->next == curr;
}

template<typename T>
bool LazyList<T>::empty() const
{
  return *head == LazyNode<T>(MinValue::get<T>()) && *(head->next) == LazyNode<T>(MaxValue::get<T>());
}
