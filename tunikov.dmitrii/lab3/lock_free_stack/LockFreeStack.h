//
// Created by dmitrii on 15.12.2019.
//

#ifndef LAB3_LOCKFREESTACK_H
#define LAB3_LOCKFREESTACK_H


#include "../interfaces/Stack.h"
#include "../node/Node.h"
#include <pthread.h>
#include <atomic>

class LockFreeStack : public  Stack {
private:
    std::atomic<int> threads_calling_pop{};
    std::atomic<Node*> head{};
    std::atomic<Node*> nodes_to_delete{};

    void try_delete_nodes(Node* deleted_node);
    static void delete_nodes(Node* node);
    void add_new_node_to_delete(Node *first, Node* last);
    void add_new_nodes_to_delete(Node *nodes);
public:
    LockFreeStack();
    ~LockFreeStack();
    std::shared_ptr<int> pop() override ;
    void push(const int& val) override ;
    bool empty() override ;
};


#endif //LAB3_LOCKFREESTACK_H
