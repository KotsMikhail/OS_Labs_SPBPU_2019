#ifndef OPTIMISTIC_LIST_H
#define OPTIMISTIC_LIST_H

#include <set.h>
#include <collector.h>

template<typename T>
class SetCreator;

template<typename T>
class OptimisticList: public Set<T>
{
public:
  class Node
  {
  public:
    T item;
    int key;
    pthread_mutex_t mutex;
    Node *next;

    static Node* create(const T& item);
    bool operator==(const Node& other) const;
    void lock();
    void unlock();
    ~Node();
  protected:
    Node(const T& item, int key, pthread_mutex_t& mutex) noexcept;
  };

private:
  friend SetCreator<T>;

  Node* head;
  Collector<Node> collector;
  const std::string tag = "OptimisticList";

  OptimisticList(Node* head, const Collector<Node>& collector);

public:
  ~OptimisticList();
  bool add(T element) override;
  bool remove(T element) override;
  bool contains(T element) const override;
  bool empty() const override;

private:
  bool validate(Node* prev, Node* curr) const;
};

#include "optimistic_list.hpp"

#endif // OPTIMISTIC_LIST_H
