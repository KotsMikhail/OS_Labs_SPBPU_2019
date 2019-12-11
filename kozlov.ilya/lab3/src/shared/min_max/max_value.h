#ifndef MAX_VALUE_H
#define MAX_VALUE_H

#include <cstdint>
#include <cfloat>

namespace MaxValue
{
  template<typename T>
  T get();

  template<>
  int get<int>()
  {
    return INT32_MAX;
  }

  template<>
  double get<double>()
  {
    return DBL_MAX;
  }

  template<>
  float get<float>()
  {
    return FLT_MAX;
  }
}

#endif // MAX_VALUE_H
