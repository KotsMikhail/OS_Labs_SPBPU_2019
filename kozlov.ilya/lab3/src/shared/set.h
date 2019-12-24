#ifndef INTERFACE_H
#define INTERFACE_H

enum class Type
{
  OPTIMISTIC,
  LAZY
};

template<typename T>
class Set
{
public:
  virtual bool add(T element) = 0;
  virtual bool remove(T element) = 0;
  virtual bool contains(T element) const = 0;
  virtual bool empty() const = 0;
  virtual ~Set() = default;
};

#endif // INTERFACE_H
