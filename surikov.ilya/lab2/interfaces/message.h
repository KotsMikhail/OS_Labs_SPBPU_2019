#ifndef OS_LABS_SPBPU_2019_MESSAGE_H
#define OS_LABS_SPBPU_2019_MESSAGE_H

enum Status
{
    ALIVE,
    DEAD
};
enum Owner
{
    WOLF,
    GOAT
};

struct Message
{
    Owner owner;
    Status status;
    int number;

    Message(Owner ow = WOLF, Status st = ALIVE, int num = 0) : owner(ow), status(st), number(num)
    {
    }
};

#endif //OS_LABS_SPBPU_2019_MESSAGE_H
