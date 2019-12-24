#include "set_optimistic.h"

template<typename T, typename L>
SetOptimistic<T, L>::SetOptimistic(): Set<T, L>(), _mutex(PTHREAD_MUTEX_INITIALIZER)
{}

template<typename T, typename L>
SetOptimistic<T, L>::~SetOptimistic()
{
    for (typename Set<T, L>::Node *to_del : _removed)
        delete to_del;
    pthread_mutex_destroy(&_mutex);
}

template<typename T, typename L>
bool SetOptimistic<T, L>::add(const T &item)
{
    bool success = false;
    while (true)
    {
        typename Set<T, L>::Node *pred = this->_head, *curr = pred->_next;
        this->loop(pred, curr, item);
        if (validate(pred, curr))
        {
            if (item < curr->_item)
            {
                typename Set<T, L>::Node *node = new (std::nothrow) typename Set<T, L>::Node(item);
                if (node)
                {
                    node->_next = curr;
                    pred->_next = node;
                    success = true;
                }
            }
            this->unlock(pred, curr);
            break;
        }
        this->unlock(pred, curr);
    }
    return success;
}

template<typename T, typename L>
bool SetOptimistic<T, L>::remove(const T &item)
{
//    std::cout << "remove" << std::endl;
    bool success = false;
    while (true)
    {
        typename Set<T, L>::Node *pred = this->_head, *curr = pred->_next;
        this->loop(pred, curr, item);
        if (validate(pred, curr))
        {
            if (!(item < curr->_item))
            {
                pred->_next = curr->_next;
                pthread_mutex_lock(&_mutex);
                _removed.push_back(curr);
                pthread_mutex_unlock(&_mutex);
                success = true;
            }
            this->unlock(pred, curr);
            break;
        }
        this->unlock(pred, curr);
    }
    return success;
}

template<typename T, typename L>
bool SetOptimistic<T, L>::contains(const T &item)
{
//    std::cout << "contains" << std::endl;
    while (true)
    {
        typename Set<T, L>::Node *pred = this->_head, *curr = pred->_next;
        this->loop(pred, curr, item);
        if (validate(pred, curr))
        {
            this->unlock(pred, curr);
            return !(item < curr->_item);
        }
        this->unlock(pred, curr);
    }
    return false;
}

template<typename T, typename L>
bool SetOptimistic<T, L>::validate(typename Set<T, L>::Node *pred, typename Set<T, L>::Node *curr)
{
//    std::cout << "validate" << std::endl;
    typename Set<T, L>::Node *node = this->_head;
    while (node->_item < pred->_item) {
        node = node->_next;
    }
    return (node == pred && pred->_next == curr);
}

