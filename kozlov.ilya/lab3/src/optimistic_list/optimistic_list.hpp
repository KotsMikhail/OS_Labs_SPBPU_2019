#include <logger/logger.h>
#include <min_max/min_value.h>
#include <min_max/max_value.h>

template<typename T>
OptimisticList<T>::Node::Node(const T& item, int key, pthread_mutex_t& mutex) noexcept:
  item(item), key(key), mutex(mutex), next(nullptr)
{
}

template<typename T>
bool OptimisticList<T>::Node::operator==(const Node& other) const
{
  return key == other.key && item == other.item;
}

template<typename T>
void OptimisticList<T>::Node::lock()
{
  if (pthread_mutex_lock(&mutex) != 0)
  {
    throw std::runtime_error("Locking mutex error");
  }
}

template<typename T>
void OptimisticList<T>::Node::unlock()
{
  if (pthread_mutex_unlock(&mutex) != 0)
  {
    throw std::runtime_error("Unlocking mutex error");
  }
}

template<typename T>
OptimisticList<T>::Node::~Node()
{
  pthread_mutex_destroy(&mutex);
}

template<typename T>
typename OptimisticList<T>::Node* OptimisticList<T>::Node::create(const T& item)
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
  return new Node(item, hash, mutex);
}

template<typename T>
OptimisticList<T>::OptimisticList(Node* head, const Collector<Node>& collector):
  head(head), collector(collector)
{
}

template<typename T>
OptimisticList<T>::~OptimisticList()
{
  Logger::logDebug(tag, "destructing...");
  Node* curr = head;
  while (curr != nullptr)
  {
    Node* prev = curr;
    curr = curr->next;
    delete prev;
  }
}

template<typename T>
bool OptimisticList<T>::add(T element)
{
  Logger::logDebug(tag, "add(" + std::to_string(element) + ")");
  int key = std::hash<T>()(element);
  bool res = false;
  bool need_return = false;
  while (true)
  {
    Node* prev = head;
    Node* curr = prev->next;
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
        auto node = Node::create(element);
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
bool OptimisticList<T>::remove(T element)
{
  Logger::logDebug(tag, "remove(" + std::to_string(element) + ")");
  int key = std::hash<T>()(element);
  bool res = false;
  bool need_return = false;
  while (true)
  {
    Node* prev = head;
    Node* curr = prev->next;
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
bool OptimisticList<T>::contains(T element) const
{
  Logger::logDebug(tag, "contains(" + std::to_string(element) + ")");
  int key = std::hash<T>()(element);
  bool res = false;
  bool need_return = false;
  while (true)
  {
    Node* prev = head;
    Node* curr = prev->next;
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
      res = curr->key == key;
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
bool OptimisticList<T>::validate(Node* prev, Node* curr) const
{
  //Logger::logDebug(tag, "validating...");
  Node* node = head;
  while (node->key <= prev->key)
  {
    if (node == prev)
    {
      return node->next == curr;
    }
    node = node->next;
  }
  return false;
}

template<typename T>
bool OptimisticList<T>::empty() const
{
  return head->item == MinValue::get<T>() && head->next->item == MaxValue::get<T>();
}
