//
// Created by nero on 23.11.2019.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

enum Status {
    ALIVE,
    DEAD
};

enum Owner {
    WOLF,
    GOAT
};

struct message {
    Owner owner;
    Status status;
    int number;

    message(Owner ow = WOLF, int num = 0, Status st = ALIVE) : owner(ow), status(st), number(num) {}
};


#endif // MESSAGE_H