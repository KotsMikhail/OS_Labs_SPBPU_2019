#ifndef NODE_H
#define NODE_H

#include <sys/types.h>
#include <pthread.h>
#include <functional>

template<typename T>
class NodeCreator;

template<typename T>
class Node
{
private:
  friend NodeCreator<T>;

  const double timeout = 1.0;

public:
  T item;
  int key;
  pthread_mutex_t mutex;
  Node *next;

  bool operator==(const Node& other) const;
  int timedLock();
  int unlock();
  ~Node();

protected:
  Node(const T& item, int key, pthread_mutex_t& mutex) noexcept;
};

#include "node.hpp"

#endif // NODE_H
