#ifndef LAB2_WOLF_INFO_WOLF_H
#define LAB2_WOLF_INFO_WOLF_H

#include "conn.h"
#include "message.h"
#include <semaphore.h>
#include <fcntl.h>
class connection_info_wolf{
public:
    pid_t pid;
    sem_t* sem;
    Status goatStatus;
};

#endif //LAB2_WOLF_INFO_WOLF_H
