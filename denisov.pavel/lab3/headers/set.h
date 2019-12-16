#ifndef __SET_H__
#define __SET_H__

#include <pthread.h>

template <typename ELEMENT_TYPE>
class SET
{
public:
    virtual ~SET (void)  {}

    virtual bool Add      (const ELEMENT_TYPE & item)       = 0;
    virtual bool Remove   (const ELEMENT_TYPE & item)       = 0;
    virtual bool Contains (const ELEMENT_TYPE & item) const = 0;
    virtual bool IsEmpty  (void)                            = 0;

protected:
    SET (void) {};
    SET (const SET&) = delete;
    SET & operator= (const SET&) = delete;
};

#endif // __SET_H__
