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
    static void start_game();
    static void init_game();
    static void *catch_goats(void *param);
    static void *eat_goats(void *param);
    static wolf *instance;
    static std::map<conn*, pid_t> pids;
    static std::map<conn*, sem_t*> connections;
    static std::map<conn*, Status> states;
    static std::map<conn*, int> goats_id;
    static std::map<conn*, message> goats_messages;
    static const int TIMEOUT = 5;
    static constexpr double WOLF_WORK_WINDOW = 1;
    static int num_goats;
    static int num;
};


#endif //LAB2_WOLF_H
