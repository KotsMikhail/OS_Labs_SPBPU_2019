#include "conn.h"
#include <semaphore.h>

#ifndef LAB2_CONNECTION_INFO_GOAT_H
#define LAB2_CONNECTION_INFO_GOAT_H

class connection_info_goat {
public:
    conn *connection;
    sem_t *semaphore;

    connection_info_goat(conn *c, sem_t *sem) : connection(c), semaphore(sem) {}
};

#endif //LAB2_CONNECTION_INFO_GOAT_H
