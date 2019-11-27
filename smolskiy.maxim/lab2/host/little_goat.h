#ifndef LITTLE_GOAT_H
#define LITTLE_GOAT_H

#include "conn.h"

class LittleGoat
{
public:
    static LittleGoat& GetInstance();
    void MakeMove();
    void FindOutStatus();

private:
    const int ALIVE_MAX = 100;
    const int DEAD_MAX = 50;
    
    Conn conn;
    int isAlive;

    LittleGoat();
    LittleGoat(const LittleGoat&) = delete;
    LittleGoat& operator=(const LittleGoat&) = delete;
};

#endif //LITTLE_GOAT_H
