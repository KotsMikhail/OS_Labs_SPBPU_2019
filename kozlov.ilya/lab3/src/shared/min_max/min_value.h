#ifndef MIN_VALUE_H
#define MIN_VALUE_H

#include <cstdint>
#include <cfloat>

namespace MinValue
{
  template<typename T>
  T get();

  template<>
  int get<int>()
  {
    return INT32_MIN;
  }

  template<>
  double get<double>()
  {
    return -DBL_MAX;
  }

  template<>
  float get<float>()
  {
    return -FLT_MAX;
  }
}

#endif // MIN_VALUE_H
