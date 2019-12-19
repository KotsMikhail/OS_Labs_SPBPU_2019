//
// Created by nero on 21.11.2019.
//

#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include "conn.h"
#include "goat.h"
#include <semaphore.h>
#include <map>

class wolf {
public:
    static wolf* get_instance();
    void start();
private:
    static wolf *instance;
    static void *catch_goats(void *param);
    static void *eat_goats(void *param);
    static void kill_client(conn* connection);
    static std::map<conn*, goat*> clients;
    static std::map<conn*, message> clients_messages;
    static int num_goats;
    static int num;
    static const int TIMEOUT = 5;
    static constexpr double WOLF_WORK_WINDOW = 1;
    wolf();
    void start_game();
    void init_game();
    void process_threads(void *(*start_routine) (void *));
};


#endif //LAB2_WOLF_H
