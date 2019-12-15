//
// Created by dmitrii on 15.12.2019.
//

#ifndef LAB3_NODE_H
#define LAB3_NODE_H

#include <memory>

struct Node {
    std::shared_ptr<int> m_data;
    Node* m_next;
    explicit Node(const int& data);
};


#endif //LAB3_NODE_H
