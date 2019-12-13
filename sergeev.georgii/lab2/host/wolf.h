//
// Created by nero on 21.11.2019.
//

#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include "conn.h"
#include <semaphore.h>
#include <map>

class wolf {
public:
    static wolf* get_instance();
    static void start();
private:
    wolf();
    static void* work(void *param);
    static wolf *instance;
    static std::map<conn*, pid_t> pids;
    static std::map<conn*, sem_t*> connections;
    static std::map<conn*, Status> states;
    static std::map<conn*, int> goats_id;
    static const int TIMEOUT = 5;
};


#endif //LAB2_WOLF_H
