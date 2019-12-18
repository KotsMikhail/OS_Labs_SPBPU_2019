#ifndef SET_H
#define SET_H

template <typename T>
class Set
{
public:
    virtual bool add(const T &item) = 0;
    virtual bool remove(const T &item) = 0;
    virtual bool contains(const T &item) = 0;
    virtual bool empty() = 0;

    virtual ~Set()
    {};
};

#endif // SET_H
