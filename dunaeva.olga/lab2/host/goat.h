#ifndef GOAT_H
#define GOAT_H

#include "conn.h"

class Goat
{
public:
    static Goat& GetInstance();
    void MakeMove();
    void FindOutStatus();

private:
    const int ALIVE_MAX = 100;
    const int DEAD_MAX = 50;
    
    Conn conn;
    int isAlive;

    Goat();
    Goat(const Goat&) = delete;
    Goat& operator=(const Goat&) = delete;
};

#endif //GOAT_H
