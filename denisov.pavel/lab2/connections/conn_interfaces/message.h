#ifndef __MESSAGE_H__
#define __MESSAGE_H__

const int TIMEOUT = 5;


enum MSG_STATUS
{
    ALIVE,
    DEAD
};


struct Message
{
    MSG_STATUS status;
    int        number;

    Message (MSG_STATUS st = MSG_STATUS::ALIVE, int num = 0)
    : status(st)
    , number(num)
    {
    }
};

#endif //__MESSAGE_H__
