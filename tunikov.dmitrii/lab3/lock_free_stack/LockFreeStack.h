#include "../stack/Stack.h"

class LockFreeStack : public Stack {

private:
    struct LockFreeNode;

    struct CounterNodePtr {
        int m_external_count;
        LockFreeNode* m_ptr;

        CounterNodePtr() : m_external_count(0), m_ptr(nullptr){}
        CounterNodePtr(LockFreeNode* ptr) : m_external_count(0), m_ptr(ptr) {}
    }__attribute__((__aligned__(16)));

    struct LockFreeNode {
        int m_data;
        int m_internal_count;
        CounterNodePtr m_next{};

        explicit LockFreeNode(int const& data_) : m_data(data_), m_internal_count(0), m_next(nullptr) {}
    };
    CounterNodePtr head{};
    void increase_head_count(CounterNodePtr& old_counter);
public:
    LockFreeStack() : head(nullptr){}
    ~LockFreeStack() override;
    void push(int const& data) override;
    int pop() override;
    bool empty() override ;
};