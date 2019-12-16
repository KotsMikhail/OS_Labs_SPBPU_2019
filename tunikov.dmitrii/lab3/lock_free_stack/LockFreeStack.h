#include <memory>
#include <atomic>
#include "../stack/Stack.h"

class LockFreeStack : public Stack {

private:
    struct LockFreeNode;

    struct CounterNodePtr {
        int m_external_count;
        LockFreeNode* m_ptr;
    };

    struct LockFreeNode {
        std::shared_ptr<int> m_data;
        std::atomic<int> m_internal_count;
        CounterNodePtr m_next{};

        explicit LockFreeNode(int const& data_) : m_data(std::make_shared<int>(data_)), m_internal_count(0) {}
    };
    std::atomic<CounterNodePtr> head{};
    void increase_head_count(CounterNodePtr& old_counter);
public:
    LockFreeStack();
    ~LockFreeStack() override;
    void push(int const& data) override;
    std::shared_ptr<int> pop() override;
    bool empty() override ;
};