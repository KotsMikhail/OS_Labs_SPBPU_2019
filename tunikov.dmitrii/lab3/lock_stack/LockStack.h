//
// Created by dmitrii on 14.12.2019.
//

#ifndef LAB3_LOCKSTACK_H
#define LAB3_LOCKSTACK_H

#include <vector>
#include <pthread.h>
#include "../interfaces/Stack.h"
#include <memory>

class LockStack : public Stack {
private:
    std::vector<int> m_data;
    pthread_mutex_t m_mutex{};

    void timed_lock();
    LockStack();
    explicit LockStack(pthread_mutex_t& mutex);
public:
    static LockStack* make();
    ~LockStack();
    void push(const int &val) override;
    std::shared_ptr<int> pop() override;
    bool empty() override;
};

#endif //LAB3_LOCKSTACK_H
