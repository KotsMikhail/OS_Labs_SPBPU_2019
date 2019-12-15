#ifndef NODE_H
#define NODE_H

#include <sys/types.h>
#include <pthread.h>
#include <functional>

template<typename T>
class Node
{
public:
  T item;
  int key;
  pthread_mutex_t mutex;
  Node *next;

  Node(const T& item, int key, pthread_mutex_t& mutex) noexcept;
  bool operator==(const Node& other) const;
  int timedLock();
  int unlock();
  ~Node();

private:
  const int timeout = 1;
};

#include "node.hpp"

#endif // NODE_H
