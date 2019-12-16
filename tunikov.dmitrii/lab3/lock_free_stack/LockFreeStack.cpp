//
// Created by dmitrii on 15.12.2019.
//

#include <new>
#include <iostream>
#include "LockFreeStack.h"

void LockFreeStack::push(const int &val) {
    Node* new_elem = new (std::nothrow) Node(val);
    new_elem->m_next = head.load();

    while (!head.compare_exchange_weak(new_elem->m_next, new_elem))
        ;
}

std::shared_ptr<int> LockFreeStack::pop() {
    threads_calling_pop++;
    Node* old_head = head.load();

    while (old_head && !head.compare_exchange_weak(old_head, old_head->m_next))
        ;

    std::shared_ptr<int> res = std::make_shared<int>();
    if (old_head)
    {
        res.swap(old_head->m_data);
    }

    try_delete_nodes(old_head);

    return res;
}

LockFreeStack::LockFreeStack()= default;

bool LockFreeStack::empty() {
    return head == nullptr;
}

void LockFreeStack::try_delete_nodes(Node *new_node_to_delete) {

    if (threads_calling_pop == 1)
    {
        Node* cur_thread_nodes_to_delete = nodes_to_delete.exchange(nullptr);
        threads_calling_pop--;
        if (threads_calling_pop == 0)
        {
            Node::delete_nodes(cur_thread_nodes_to_delete);
        }
        else if (nodes_to_delete)
        {
            add_new_nodes_to_delete(cur_thread_nodes_to_delete);
        }

        delete new_node_to_delete;
    }
    else
    {
        add_new_node_to_delete(new_node_to_delete, new_node_to_delete);
        threads_calling_pop--;
    }
}

void LockFreeStack::add_new_node_to_delete(Node *first, Node* last)
{
    if (!first || !last)
        return;

    last->m_next = nodes_to_delete;

    while (!nodes_to_delete.compare_exchange_weak(last->m_next, first))
        ;
}

void LockFreeStack::add_new_nodes_to_delete(Node *nodes) {
    if (nodes == nullptr)
        return;

    Node* last = nodes;

    while (last->m_next)
    {
        last = last->m_next;
    }

    add_new_node_to_delete(nodes, last);
}

LockFreeStack::~LockFreeStack() {
    Node::delete_nodes(nodes_to_delete);
}
