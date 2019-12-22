//
// Created by nero on 22.11.2019.
//

#ifndef LAB2_GOAT_H
#define LAB2_GOAT_H
#include "Message.h"
#include "conn.h"
#include <unistd.h>
#include <semaphore.h>
#include <iostream>

class goat {
public:
    void start();
    goat(conn* connection, sem_t* semaphore, Status st = Status::ALIVE) {
        this->connection = connection;
        this->semaphore = semaphore;
        this->status = st;
    }
    ~goat(){
        delete this->connection;
        sem_destroy(this->semaphore);
    }
    Status get_status(){ return status; }
    conn* get_connection(){ return connection; }
    sem_t* get_semaphore(){ return semaphore; }
    pid_t get_pid(){ return pid; }
    void set_pid(pid_t p) { this->pid = p; }

private:
    pid_t pid;
    Status status;
    conn* connection;
    sem_t* semaphore;
    int generate_rand_i(int max);
};


#endif //LAB2_GOAT_H
