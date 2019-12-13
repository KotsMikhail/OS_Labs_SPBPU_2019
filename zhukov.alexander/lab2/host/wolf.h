#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include "conn.h"
#include "connection_info_wolf.h"
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
    static std::map<conn*, connection_info_wolf*> connections;

    static void signal_handler(int signum);
    static const int TIMEOUT = 5;
};


#endif //LAB2_WOLF_H
