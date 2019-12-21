#ifndef OPTIMISTIC_LIST_H
#define OPTIMISTIC_LIST_H

#include <pthread.h>

#include <vector>

#include "set.h"

template <typename T>
class OptimisticList : public Set<T>
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
    std::vector<Node*> nodes;
    pthread_mutex_t mutex;

    bool validate(Node *pred, Node *curr);
    void find(Node* &pred, Node* &curr, size_t key);

public:
    OptimisticList();

    bool add(const T &item) override;
    bool remove(const T &item) override;
    bool contains(const T &item) override;
    bool empty() override;

    ~OptimisticList();
};

#include "optimistic_list.hpp"

#endif // OPTIMISTIC_LIST_H
