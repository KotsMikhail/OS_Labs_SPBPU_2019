#pragma once
#ifndef __LAZY_SYNCHRONIZATION_SET_HPP__
#define __LAZY_SYNCHRONIZATION_SET_HPP__

#include <new>
#include <limits>
#include <iostream>

#include "lazy_synchronization_set.h"


template<typename ELEMENT_TYPE>
SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::NODE::NODE (ELEMENT_TYPE val)
    : value(val)
    , isDeleted(false)
    , next(nullptr)
{
    pthread_mutex_init(&mutex, NULL);
}


template<typename ELEMENT_TYPE>
SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::NODE::~NODE (void)
{
    pthread_mutex_destroy(&mutex);
}


template<typename ELEMENT_TYPE>
SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::SET_LAZY_SYNCHRONIZATION (const ELEMENT_TYPE &minItem, const ELEMENT_TYPE &maxItem)
{
    root = new NODE(minItem);
    root->next = new NODE(maxItem);
}


template<typename ELEMENT_TYPE>
SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE> * SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::Create ()
{
    SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE> * set = new (std::nothrow) SET_LAZY_SYNCHRONIZATION(std::numeric_limits<ELEMENT_TYPE>::min(), std::numeric_limits<ELEMENT_TYPE>::max());
    if (!set) {
        std::cout << "Failed to create Lazy-Synchronization Set" << std::endl;
    }
    return set;
}


template<typename ELEMENT_TYPE>
SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::~SET_LAZY_SYNCHRONIZATION (void)
{
    while (root != nullptr) {
      NODE *node = root;
      root = root->next;
      delete node;
    }
}


template<typename ELEMENT_TYPE>
bool SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::Add (const ELEMENT_TYPE &item)
{
    NODE *prev;
    NODE *curr;

    while (true) {
      prev = root;
      curr = root->next;

      while (curr->value < item && curr->next != nullptr) {
         prev = curr;
         curr = curr->next;
      }

      pthread_mutex_lock(&prev->mutex);
      pthread_mutex_lock(&curr->mutex);

      if (IsValidPair(prev, curr)) {
         break;
      }

      pthread_mutex_unlock(&curr->mutex);
      pthread_mutex_unlock(&prev->mutex);
    }

    bool res = false;

    if (curr->value != item) {
      NODE *node = new NODE(item);

      node->next = curr;
      prev->next = node;

      res = true;
    } else if (curr->isDeleted) {
      curr->isDeleted = false;

      res = true;
    }

    pthread_mutex_unlock(&curr->mutex);
    pthread_mutex_unlock(&prev->mutex);

    return res;
}


template<typename ELEMENT_TYPE>
bool SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::Remove (const ELEMENT_TYPE &item)
{
    NODE *prev;
    NODE *curr;
    while (true) {
      prev = root;
      curr = root->next;

      while (curr->value < item && curr->next != nullptr) {
         prev = curr;
         curr = curr->next;
      }

      pthread_mutex_lock(&prev->mutex);
      pthread_mutex_lock(&curr->mutex);

      if (IsValidPair(prev, curr)) {
         break;
      }

      pthread_mutex_unlock(&curr->mutex);
      pthread_mutex_unlock(&prev->mutex);
    }

    if (curr->value == item) {
      curr->isDeleted = true;

      prev->next = curr->next;

      pthread_mutex_unlock(&curr->mutex);
      delete curr;

      pthread_mutex_unlock(&prev->mutex);

      return true;
    }

    pthread_mutex_unlock(&curr->mutex);
    pthread_mutex_unlock(&prev->mutex);

    return false;
}


template<typename ELEMENT_TYPE>
bool SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::Contains (const ELEMENT_TYPE &item) const
{
    NODE *curr = root->next;

    while (curr->value < item && curr->next != nullptr) {
      curr = curr->next;
    }

    return curr->value == item && !curr->isDeleted;
}


template<typename ELEMENT_TYPE>
bool SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::IsValidPair (NODE *prev, NODE *curr)
{
    return !prev->isDeleted && !curr->isDeleted && prev->next == curr;
}


template<typename ELEMENT_TYPE>
bool SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::IsEmpty (void)
{
    ELEMENT_TYPE min = std::numeric_limits<ELEMENT_TYPE>::min();
    ELEMENT_TYPE max = std::numeric_limits<ELEMENT_TYPE>::max();

    NODE *nextNode = root->next;
    std::less<ELEMENT_TYPE> cmp;
    return (!cmp(root->value, min) && !cmp(min, root->value) &&
            !cmp(nextNode->value, max) && !cmp(max, nextNode->value));
}

#endif // __LAZY_SYNCHRONIZATION_SET_HPP__
