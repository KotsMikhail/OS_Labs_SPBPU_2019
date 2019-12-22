#ifndef LAZY_LIST_H
#define LAZY_LIST_H

#include <set.h>
#include <collector.h>
#include "../optimistic_list/optimistic_list.h"

template<typename T>
class SetCreator;

template<typename T>
class LazyList: public Set<T>
{
public:
  class LazyNode: public OptimisticList<T>::Node
  {
  public:
    bool marked;
    LazyNode *next;

    static LazyNode* create(const T& item);
  private:
    LazyNode(const T& item, int key, pthread_mutex_t& mutex) noexcept:
      OptimisticList<T>::Node(item, key, mutex), marked(false), next(nullptr)
    {
    }
  };

private:
  friend SetCreator<T>;

  LazyNode* head;
  Collector<LazyNode> collector;
  const std::string tag = "LazyList";

  LazyList(LazyNode* head, const Collector<LazyNode>& collector);

public:
  ~LazyList();
  bool add(T element) override;
  bool remove(T element) override;
  bool contains(T element) const override;
  bool empty() const override;

private:
  bool validate(LazyNode* prev, LazyNode* curr) const;
};

#include "lazy_list.hpp"

#endif // LAZY_LIST_H