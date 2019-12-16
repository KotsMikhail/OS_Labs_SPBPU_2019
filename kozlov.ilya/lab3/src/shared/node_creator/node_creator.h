#ifndef NODE_CREATOR_H
#define NODE_CREATOR_H

#include <node/node.h>
#include <logger/logger.h>
#include "../../lazy_list/lazy_node.h"

template<typename T>
class NodeCreator
{
private:
  bool failed;
  pthread_mutex_t mutex;
  int hash;
  T item;
  const std::string tag = "NodeCreator";

  bool initData();
public:
  explicit NodeCreator(const T& item);
  template<class NodeT>
  NodeT* get();
};

#include "node_creator.hpp"

#endif // NODE_CREATOR_H
