#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include "conn.h"
#include <semaphore.h>
#include <map>
#include "goat.h"

class wolf {
public:
    static wolf *get_instance();

    void start();

private:
    wolf();

    static void *ReadGoat(void *param);

    static void *WriteGoat(void *param);

    void CreateGoats();

    static wolf *instance;

    void threadsWork(void *(*pFunction)(void *));

    std::map<goat *, message> clients;
    int numGoats;
    int wolfNumber;
    static const int TIMEOUT = 5;
};


#endif //LAB2_WOLF_H
