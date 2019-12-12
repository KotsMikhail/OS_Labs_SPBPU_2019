#ifndef LAZY_LIST_H
#define LAZY_LIST_H

#include <set.h>
#include <node/node.h>
#include "lazy_node.h"

template<typename T>
class LazyList: public Set<T>
{
private:
  LazyNode<T>* head;
  const std::string tag = "LazyList";
public:
  LazyList();
  ~LazyList();
  bool add(T element) override;
  bool remove(T element) override;
  bool contains(T element) const override;
private:
  bool validate(LazyNode<T>* prev, LazyNode<T>* curr) const;
};

#include "lazy_list.hpp"

#endif // LAZY_LIST_H