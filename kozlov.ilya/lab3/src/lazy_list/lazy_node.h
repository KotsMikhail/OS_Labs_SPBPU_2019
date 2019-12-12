#ifndef LAZY_NODE_H
#define LAZY_NODE_H

#include <node/node.h>

template<typename T>
struct LazyNode: public Node<T>
{
  bool marked;
  LazyNode *next;

  explicit LazyNode(T item) : Node<T>(item), marked(false), next(nullptr)
  {
  }
};

#endif // LAZY_NODE_H
