#include <functional>

template <typename T>
FineList<T>::Node::Node(const T &i, size_t k, Node *n)
    : mutex(PTHREAD_MUTEX_INITIALIZER), item(i), key(k), next(n)
{};

template <typename T>
void FineList<T>::Node::lock()
{
    pthread_mutex_lock(&mutex);
};

template <typename T>
void FineList<T>::Node::unlock()
{
    pthread_mutex_unlock(&mutex);
};

template <typename T>
FineList<T>::Node::~Node()
{
    pthread_mutex_destroy(&mutex);
};

template <typename T>
FineList<T>::FineList()
    : head(new Node(T(), 0, new Node(T(), SIZE_MAX, nullptr)))
{}

template <typename T>
bool FineList<T>::add(const T &item)
{
    size_t key = std::hash<T>()(item);
    head->lock();
    Node *pred = head, *curr = pred->next;
    curr->lock();

    while (curr->key < key)
    {
        pred->unlock();
        pred = curr;
        curr = curr->next;
        curr->lock();
    }

    if (curr->key == key)
    {
        curr->unlock();
        pred->unlock();

        return false;
    }
    
    Node *newNode = new Node(item, key, curr);
    pred->next = newNode;

    curr->unlock();
    pred->unlock();

    return true;
}

template <typename T>
bool FineList<T>::remove(const T &item)
{
    size_t key = std::hash<T>()(item);
    head->lock();
    Node *pred = head, *curr = pred->next;
    curr->lock();

    while (curr->key < key)
    {
        pred->unlock();
        pred = curr;
        curr = curr->next;
        curr->lock();
    }

    if (curr->key == key)
    {
        pred->next = curr->next;
        curr->unlock();
        delete curr;
        pred->unlock();

        return true;
    }

    curr->unlock();
    pred->unlock();

    return false;
}

template <typename T>
bool FineList<T>::contains(const T &item)
{
    size_t key = std::hash<T>()(item);
    head->lock();
    Node *pred = head, *curr = pred->next;
    curr->lock();

    while (curr->key < key)
    {
        pred->unlock();
        pred = curr;
        curr = curr->next;
        curr->lock();
    }

    if (curr->key == key)
    {
        curr->unlock();
        pred->unlock();

        return true;
    }
        
    curr->unlock();
    pred->unlock();

    return false;
}

template <typename T>
bool FineList<T>::empty()
{
    return head->key == 0 && head->next->key == SIZE_MAX;
}

template <typename T>
FineList<T>::~FineList()
{
    Node *curr = head, *next;
    while (curr)
    {
        next = curr->next;
        delete curr;
        curr = next;
    }
}
