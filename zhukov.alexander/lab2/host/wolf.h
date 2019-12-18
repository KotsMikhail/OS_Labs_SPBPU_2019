#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include "conn.h"
#include <semaphore.h>
#include <map>
#include "goat.h"

class wolf {
public:
    static wolf *get_instance();

    static void start();

private:
    wolf();

    static void *ReadGoat(void *param);

    static void *WriteGoat(void *param);

    static void CreateGoats();

    static wolf *instance;
    static std::map<conn *, goat *> connections;
    static std::map<conn *, message> messages;
    static std::map<conn *, Status> statuses;
    static int numGoats;
    static int wolfNumber;
    static const int TIMEOUT = 5;
};


#endif //LAB2_WOLF_H
