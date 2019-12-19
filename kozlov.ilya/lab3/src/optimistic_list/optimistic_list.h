#ifndef OPTIMISTIC_LIST_H
#define OPTIMISTIC_LIST_H

#include <set.h>
#include <node/node.h>
#include <node_collector.h>

template<typename T>
class SetCreator;

template<typename T>
class OptimisticList: public Set<T>
{
private:
  friend SetCreator<T>;

  Node<T>* head;
  NodeCollector<Node<T>> collector;
  const std::string tag = "OptimisticList";

  OptimisticList(Node<T>* head, const NodeCollector<Node<T>>& collector);

public:
  ~OptimisticList();
  bool add(T element) override;
  bool remove(T element) override;
  bool contains(T element) const override;
  bool empty() const override;

private:
  bool validate(Node<T>* prev, Node<T>* curr) const;
};

#include "optimistic_list.hpp"

#endif // OPTIMISTIC_LIST_H
