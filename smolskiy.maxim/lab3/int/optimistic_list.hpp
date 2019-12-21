#include <functional>

template <typename T>
OptimisticList<T>::Node::Node(const T &i, size_t k, Node *n)
    : mutex(PTHREAD_MUTEX_INITIALIZER), item(i), key(k), next(n)
{};

template <typename T>
void OptimisticList<T>::Node::lock()
{
    pthread_mutex_lock(&mutex);
};

template <typename T>
void OptimisticList<T>::Node::unlock()
{
    pthread_mutex_unlock(&mutex);
};

template <typename T>
OptimisticList<T>::Node::~Node()
{
    pthread_mutex_destroy(&mutex);
};

template <typename T>
OptimisticList<T>::OptimisticList()
    : mutex(PTHREAD_MUTEX_INITIALIZER)
{
    Node *next = new Node(T(), SIZE_MAX, nullptr);
    head = new Node(T(), 0, next);
    nodes.push_back(head);
    nodes.push_back(next);
}

template <typename T>
bool OptimisticList<T>::validate(Node *pred, Node *curr)
{
    Node *node = head;

    while (node->key <= pred->key)
    {
        if (node == pred)
            return pred->next == curr;
        
        node = node->next;
    }

    return false;
}

template <typename T>
void OptimisticList<T>::find(Node* &pred, Node* &curr, size_t key)
{
    while (curr->key < key)
    {
        pred = curr;
        curr = curr->next;
    }
}

template <typename T>
bool OptimisticList<T>::add(const T &item)
{
    size_t key = std::hash<T>()(item);

    while (true)
    {
        Node *pred = head, *curr = pred->next;
        find(pred, curr, key);
        pred->lock();
        curr->lock();

        if (validate(pred, curr))
        {
            if (curr->key == key)
            {
                pred->unlock();
                curr->unlock();

                return false;
            }
            else
            {
                Node *node = new Node(item, key, curr);
                pred->next = node;
                pthread_mutex_lock(&mutex);
                nodes.push_back(node);
                pthread_mutex_unlock(&mutex);

                pred->unlock();
                curr->unlock();

                return true;
            }
        }
        else
        {
            pred->unlock();
            curr->unlock();
        }
    }
}

template <typename T>
bool OptimisticList<T>::remove(const T &item)
{
    size_t key = std::hash<T>()(item);

    while (true)
    {
        Node *pred = head, *curr = pred->next;
        find(pred, curr, key);
        pred->lock();
        curr->lock();

        if (validate(pred, curr))
        {
            if (curr->key == key)
            {
                pred->next = curr->next;

                pred->unlock();
                curr->unlock();

                return true;
            }
            else
            {
                pred->unlock();
                curr->unlock();

                return false;
            }
        }
        else
        {
            pred->unlock();
            curr->unlock();
        }
    }
}

template <typename T>
bool OptimisticList<T>::contains(const T &item)
{
    size_t key = std::hash<T>()(item);

    while (true)
    {
        Node *pred = head, *curr = pred->next;
        find(pred, curr, key);
        pred->lock();
        curr->lock();

        if (validate(pred, curr))
        {
            pred->unlock();
            curr->unlock();

            return curr->key == key;
        }
        else
        {
            pred->unlock();
            curr->unlock();
        }
        
    }
}

template <typename T>
bool OptimisticList<T>::empty()
{
    return head->key == 0 && head->next->key == SIZE_MAX;
}

template <typename T>
OptimisticList<T>::~OptimisticList()
{
    for (Node *node : nodes)
        delete node;
    
    pthread_mutex_destroy(&mutex);
}
