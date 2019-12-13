//
// Created by nero on 22.11.2019.
//

#ifndef LAB2_GOAT_H
#define LAB2_GOAT_H
#include "Message.h"
#include "connection_info.h"

class goat {
public:
    static int start(const connection_info &c_inf);
private:
    static Status status;
    static int generate_rand_i(int max);
};


#endif //LAB2_GOAT_H
