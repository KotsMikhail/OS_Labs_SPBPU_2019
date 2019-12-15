//
// Created by dmitrii on 15.12.2019.
//

#include <new>
#include "Node.h"

Node::Node(const int &data) {
    m_data = std::make_shared<int>(data);
}
