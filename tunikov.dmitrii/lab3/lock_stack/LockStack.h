//
// Created by dmitrii on 14.12.2019.
//

#ifndef LAB3_LOCKSTACK_H
#define LAB3_LOCKSTACK_H

#include <vector>
#include <pthread.h>
#include "../stack/Stack.h"
#include "../node/Node.h"
#include <memory>

class LockStack : public Stack {
private:
    Node* head;
    pthread_mutex_t m_mutex{};

    void timed_lock();
    LockStack() = delete;
    LockStack(const LockStack& lock_stack) = delete;
    LockStack&operator=(const LockStack& h) = delete;
    explicit LockStack(pthread_mutex_t& mutex);
public:
    static LockStack* make();
    ~LockStack();
    void push(const int &val) override;
    std::shared_ptr<int> pop() override;
    bool empty() override;
};

#endif //LAB3_LOCKSTACK_H
