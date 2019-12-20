#ifndef LAB2_GOAT_H
#define LAB2_GOAT_H

#include "message.h"
#include "conn.h"
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

class goat {
public:
    int start();

    goat(Status st, conn *c, sem_t *sem) : status(st),connection(c), semaphore(sem) {}
    Status getStatus();
    sem_t* getSemaphore();
    pid_t getPid();
    conn* getConnection();
    void setPid(pid_t p){pid = p;}
private:
    Status status;
    conn* connection;
    sem_t *semaphore;
    pid_t pid;


    int GenerateRandNum(int max);
};


#endif //LAB2_GOAT_H
