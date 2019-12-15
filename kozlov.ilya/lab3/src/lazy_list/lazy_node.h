#ifndef LAZY_NODE_H
#define LAZY_NODE_H

#include <node/node.h>

template<typename T>
class LazyNode: public Node<T>
{
public:
  bool marked;
  LazyNode *next;

  LazyNode(const T& item, int key, pthread_mutex_t& mutex) noexcept:
    Node<T>(item, key, mutex), marked(false), next(nullptr)
  {
  }
};

#endif // LAZY_NODE_H
