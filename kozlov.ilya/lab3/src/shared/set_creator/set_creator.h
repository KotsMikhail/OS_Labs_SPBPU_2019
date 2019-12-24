#ifndef SET_GENERATOR_H
#define SET_GENERATOR_H

#include <logger/logger.h>
#include "set.h"
#include "../../optimistic_list/optimistic_list.h"
#include "../../lazy_list/lazy_list.h"

template<typename T>
class SetCreator
{
private:
  static const std::string tag;
public:
  static Set<T>* get(const Type& type);
};

#include "set_creator.hpp"

#endif // SET_GENERATOR_H
