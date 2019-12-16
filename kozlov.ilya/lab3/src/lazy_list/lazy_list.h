#ifndef LAZY_LIST_H
#define LAZY_LIST_H

#include <set.h>
#include <node/node.h>
#include "lazy_node.h"
#include <node_collector.h>

template<typename T>
class LazyList: public Set<T>
{
private:
  LazyNode<T>* head;
  NodeCollector<LazyNode<T>> collector;
  const std::string tag = "LazyList";

public:
  LazyList(LazyNode<T>* head, const NodeCollector<LazyNode<T>>& collector);
  ~LazyList();
  bool add(T element) override;
  bool remove(T element) override;
  bool contains(T element) const override;
  bool empty() const override;

private:
  bool validate(LazyNode<T>* prev, LazyNode<T>* curr) const;
};

#include "lazy_list.hpp"

#endif // LAZY_LIST_H