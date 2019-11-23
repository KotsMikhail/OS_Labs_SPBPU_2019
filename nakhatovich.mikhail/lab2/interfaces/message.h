#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#define SEM_HOST_NAME   "host_semaphore"
#define SEM_CLIENT_NAME "client_semaphore"

struct message_t
{
    uint32_t day, mon, year;
    int temp;

    message_t(uint32_t day=0, uint32_t mon=0, uint32_t year=0, int temp=0) 
        : day(day), mon(mon), year(year), temp(temp)
    {};
};

const int MESSAGE_SIZE = sizeof(message_t);
const int TIMEOUT = 5;

#endif // MESSAGE_H
