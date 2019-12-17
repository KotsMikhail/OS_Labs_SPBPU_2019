//
// Created by dmitrii on 14.12.2019.
//

#ifndef LAB3_LOCKSTACK_H
#define LAB3_LOCKSTACK_H

#include <vector>
#include <pthread.h>
#include "../stack/Stack.h"
#include <memory>

class LockStack : public Stack {
private:
    struct Node {
        std::shared_ptr<int> m_data;
        Node* m_next;
        explicit Node(const int& data);
        static void delete_nodes(Node* node);
    };

    Node* head;
    pthread_mutex_t m_mutex{};

    void timed_lock();
    explicit LockStack(pthread_mutex_t& mutex);
public:
    LockStack() = delete;
    LockStack(const LockStack& lock_stack) = delete;
    LockStack&operator=(const LockStack& h) = delete;
    static LockStack* make();
    ~LockStack() override ;
    void push(const int &val) override;
    std::shared_ptr<int> pop() override;
    bool empty() override;
};

#endif //LAB3_LOCKSTACK_H
