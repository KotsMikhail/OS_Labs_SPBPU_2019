#ifndef __FINE_GRAINED_SET_H__
#define __FINE_GRAINED_SET_H__

#include "set.h"

template <typename ELEMENT_TYPE>
class SET_FINE_GRAINED
    : public SET<ELEMENT_TYPE>
{
public:
    ~SET_FINE_GRAINED (void);

    bool Add      (const ELEMENT_TYPE &item);
    bool Remove   (const ELEMENT_TYPE &item);
    bool Contains (const ELEMENT_TYPE &item) const;
    bool IsEmpty  (void);

   static SET_FINE_GRAINED<ELEMENT_TYPE> * Create ();
private:
    struct NODE
    {
       NODE  (ELEMENT_TYPE val);
       ~NODE (void);


       ELEMENT_TYPE    value;
       NODE *          next;
       pthread_mutex_t mutex;
    };

    NODE *root;

    SET_FINE_GRAINED (const ELEMENT_TYPE &minItem, const ELEMENT_TYPE &maxItem);
};

#include "fine_grained_set.hpp"

#endif // __FINE_GRAINED_SET_H__
