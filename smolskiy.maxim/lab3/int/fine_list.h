#ifndef FINE_LIST_H
#define FINE_LIST_H

#include <pthread.h>

#include "set.h"

template <typename T>
class FineList : public Set<T>
{
private:
    class Node
    {
    private:
        pthread_mutex_t mutex;

    public:
        T item;
        size_t key;
        Node *next;

        Node(const T &i, size_t k, Node *n);

        void lock();
        void unlock();

        ~Node();
    };

    Node *head;

    void find(Node* &pred, Node* &curr, size_t key);

public:
    FineList();

    bool add(const T &item) override;
    bool remove(const T &item) override;
    bool contains(const T &item) override;
    bool empty() override;

    ~FineList();
};

#include "fine_list.hpp"

#endif // FINE_LIST_H
