#ifndef LAZY_NODE_H
#define LAZY_NODE_H

#include <node/node.h>

template<typename T>
class NodeCreator;

template<typename T>
class LazyNode: public Node<T>
{
private:
  friend NodeCreator<T>;

  LazyNode(const T& item, int key, pthread_mutex_t& mutex) noexcept:
    Node<T>(item, key, mutex), marked(false), next(nullptr)
  {
  }

public:
  bool marked;
  LazyNode *next;
};

#endif // LAZY_NODE_H
