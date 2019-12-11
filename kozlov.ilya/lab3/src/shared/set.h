#ifndef INTERFACE_H
#define INTERFACE_H

template<typename T>
class Set
{
public:
  virtual bool add(T element) = 0;
  virtual bool remove(T element) = 0;
  virtual bool contains(T element) const = 0;
};

#endif // INTERFACE_H
