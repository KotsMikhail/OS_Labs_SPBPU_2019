#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

enum class Status {
    ALIVE,
    DEAD
};

enum class Owner {
    WOLF,
    GOAT
};

struct message {
    Owner owner;
    Status status;
    int number;

    message(Owner ow = Owner::WOLF, int num = 0, Status st = Status::ALIVE) : owner(ow), status(st), number(num) {}
};


#endif // MESSAGE_H