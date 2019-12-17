#ifndef SET_H
#define SET_H

#include <functional>

#include "min_max.h"

template<class t, class limits = limits_t<t>, class compare = std::less<t>>
class set_t
{
public:
    virtual ~set_t();

    virtual bool add(const t &item) = 0;
    virtual bool remove(const t &item) = 0;
    virtual bool contains(const t &item) = 0;

    bool empty();

protected:
    class node_t
    {
    public:
        node_t(const t &item);
        ~node_t();

        int lock();
        int unlock();

        t item;
        node_t *next;

    private:
        node_t() = delete;
        node_t(node_t const&) = delete;
        node_t& operator=(node_t const&) = delete;

        pthread_mutex_t _lock;
    };

    set_t() = delete;
    set_t(set_t const&) = delete;
    set_t& operator=(set_t const&) = delete;

    set_t(node_t *head);

    static node_t * create_head();

    void unlock(node_t *pred, node_t *curr);
    virtual void loop(node_t *&pred, node_t *&curr, const t &item) = 0;

    node_t *_head;
    compare _cmp;
};

#include "set.hpp"

#endif // SET_H
