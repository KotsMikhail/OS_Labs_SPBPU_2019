//
// Created by dmitrii on 14.12.2019.
//

#ifndef LAB3_BLOCKINGSTACK_H
#define LAB3_BLOCKINGSTACK_H

#include <vector>
#include <pthread.h>
#include "../interfaces/Stack.h"

class BlockingStack : public Stack {
private:
    std::vector<int> m_data;
    pthread_mutex_t m_mutex{};

    void timed_lock();
    BlockingStack& operator=(const BlockingStack&h);

    BlockingStack();
    explicit BlockingStack(pthread_mutex_t& mutex);

public:
    static BlockingStack make();
    ~BlockingStack();
    void push(const int& val) override;
    void pop(int &val) override;
    bool empty() override;

    BlockingStack(const BlockingStack& h);
};

#endif //LAB3_BLOCKINGSTACK_H
