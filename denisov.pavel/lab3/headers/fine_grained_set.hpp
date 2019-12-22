#ifndef __FINE_GRAINED_SET_HPP__
#define __FINE_GRAINED_SET_HPP__

#include <new>
#include <limits>
#include <iostream>
#include <functional>

#include "fine_grained_set.h"


template<typename ELEMENT_TYPE>
SET_FINE_GRAINED<ELEMENT_TYPE>::NODE::NODE (ELEMENT_TYPE val)
    : value(val)
    , next(nullptr)
{
    pthread_mutex_init(&mutex, NULL);
}


template<typename ELEMENT_TYPE>
SET_FINE_GRAINED<ELEMENT_TYPE>::NODE::~NODE (void)
{
    pthread_mutex_destroy(&mutex);
}


template<typename ELEMENT_TYPE>
SET_FINE_GRAINED<ELEMENT_TYPE>::SET_FINE_GRAINED (const ELEMENT_TYPE &minItem, const ELEMENT_TYPE &maxItem)
{
    root = new NODE(minItem);
    root->next = new NODE(maxItem);
}


template<typename ELEMENT_TYPE>
SET_FINE_GRAINED<ELEMENT_TYPE> * SET_FINE_GRAINED<ELEMENT_TYPE>::Create (void)
{
    SET_FINE_GRAINED<ELEMENT_TYPE> * set = new (std::nothrow) SET_FINE_GRAINED<ELEMENT_TYPE>(std::numeric_limits<ELEMENT_TYPE>::min(), std::numeric_limits<ELEMENT_TYPE>::max());
    if (!set) {
        std::cout << "Failed to create Fine-Grained Set" << std::endl;
    }
    return set;
}


template<typename ELEMENT_TYPE>
SET_FINE_GRAINED<ELEMENT_TYPE>::~SET_FINE_GRAINED (void)
{
    while (root != nullptr) {
      NODE *node = root;
      root = root->next;
      delete node;
    }
}


template<typename ELEMENT_TYPE>
bool SET_FINE_GRAINED<ELEMENT_TYPE>::Add (const ELEMENT_TYPE &item)
{
    pthread_mutex_lock(&root->mutex);
    pthread_mutex_lock(&root->next->mutex);

    NODE *prev = root;
    NODE *curr = root->next;

    while (curr->value < item && curr->next != nullptr) {
      pthread_mutex_unlock(&prev->mutex);
      prev = curr;
      curr = curr->next;
      pthread_mutex_lock(&curr->mutex);
    }

    bool res = false;

    if (curr->value != item) {
      NODE *node = new NODE(item);
      node->next = curr;
      prev->next = node;

      res = true;
    }

    pthread_mutex_unlock(&curr->mutex);
    pthread_mutex_unlock(&prev->mutex);

    return res;
}


template<typename ELEMENT_TYPE>
bool SET_FINE_GRAINED<ELEMENT_TYPE>::Remove (const ELEMENT_TYPE &item)
{
    pthread_mutex_lock(&root->mutex);
    pthread_mutex_lock(&root->next->mutex);

    NODE *prev = root;
    NODE *curr = root->next;

    while (curr->value < item && curr->next != nullptr) {
      pthread_mutex_unlock(&prev->mutex);
      prev = curr;
      curr = curr->next;
      pthread_mutex_lock(&curr->mutex);
    }


    if (curr->value == item) {
      prev->next = curr->next;
      delete curr;

      pthread_mutex_unlock(&prev->mutex);
      return true;
    }

    pthread_mutex_unlock(&curr->mutex);
    pthread_mutex_unlock(&prev->mutex);

    return false;
}


template<typename ELEMENT_TYPE>
bool SET_FINE_GRAINED<ELEMENT_TYPE>::Contains (const ELEMENT_TYPE &item) const
{
    pthread_mutex_lock(&root->mutex);
    pthread_mutex_lock(&root->next->mutex);

    NODE *prev = root;
    NODE *curr = root->next;

    while (curr->value < item && curr->next != nullptr) {
      pthread_mutex_unlock(&prev->mutex);
      prev = curr;
      curr = curr->next;
      pthread_mutex_lock(&curr->mutex);
    }

    bool res = false;

    if (curr->value == item) {
      res = true;
    }

    pthread_mutex_unlock(&curr->mutex);
    pthread_mutex_unlock(&prev->mutex);

    return res;
}


template<typename ELEMENT_TYPE>
bool SET_FINE_GRAINED<ELEMENT_TYPE>::IsEmpty (void)
{
    ELEMENT_TYPE min = std::numeric_limits<ELEMENT_TYPE>::min();
    ELEMENT_TYPE max = std::numeric_limits<ELEMENT_TYPE>::max();

    NODE *nextNode = root->next;
    std::less<ELEMENT_TYPE> cmp;
    return (!cmp(root->value, min) && !cmp(min, root->value) &&
            !cmp(nextNode->value, max) && !cmp(max, nextNode->value));
}

#endif // __FINE_GRAINED_SET_HPP__
