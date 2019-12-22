#ifndef __LAZY_SYNCHRONIZATION_SET_H__
#define __LAZY_SYNCHRONIZATION_SET_H__

#include "set.h"

template <typename ELEMENT_TYPE>
class SET_LAZY_SYNCHRONIZATION
    : public SET<ELEMENT_TYPE>
{
public:
    ~SET_LAZY_SYNCHRONIZATION (void);

    bool Add      (const ELEMENT_TYPE &item);
    bool Remove   (const ELEMENT_TYPE &item);
    bool Contains (const ELEMENT_TYPE &item) const;
    bool IsEmpty  (void);

    static SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE> * Create ();
private:
    struct NODE
    {
       NODE  (ELEMENT_TYPE val);
       ~NODE (void);


       ELEMENT_TYPE    value;
       bool            isDeleted;
       NODE *          next;
       pthread_mutex_t mutex;
    };

    NODE *root;

    bool IsValidPair         (NODE *prev, NODE *curr);
    SET_LAZY_SYNCHRONIZATION (const ELEMENT_TYPE &minItem, const ELEMENT_TYPE &maxItem);
};

#include "lazy_synchronization_set.hpp"

#endif // __LAZY_SYNCHRONIZATION_SET_H__
