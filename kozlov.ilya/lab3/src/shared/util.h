#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sstream>

namespace Util
{
  template<typename T>
  std::string toStr(T begin, T end)
  {
    std::stringstream ss;
    bool first = true;
    for (; begin != end; begin++)
    {
      if (!first)
      {
        ss << ", ";
      }
      ss << *begin;
      first = false;
    }
    return ss.str();
  }

  enum class Color
  {
    RED = 31,
    GREEN = 32,
    BLUE = 34,
    DEFAULT = 39
  };

  class Modifier
  {
    Color code;
  public:
    Modifier(Color pCode): code(pCode) {}

    friend std::ostream&
    operator<<(std::ostream& os, const Modifier& mod)
    {
      return os << "\033[" << (size_t)mod.code << "m";
    }
  };
}

#endif // UTIL_H
