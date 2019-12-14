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

    static std::vector<int> m_data;
    static pthread_mutex_t m_mutex;
    static BlockingStack* m_inst;
    static int m_read_timeout;

    void timed_lock() const;
    BlockingStack& operator=(const BlockingStack&h);
    BlockingStack(const BlockingStack& h);
    BlockingStack();
    explicit BlockingStack(pthread_mutex_t& mutex);
    ~BlockingStack();
public:
    static void destroyStack();
    static void initStack();
    static BlockingStack& getInstance();
    void push(const int& val) override;
    void pop(int &val) override;
    bool empty() const override;
};

#endif //LAB3_BLOCKINGSTACK_H
