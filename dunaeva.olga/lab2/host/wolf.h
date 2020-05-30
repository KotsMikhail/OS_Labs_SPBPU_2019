#ifndef WOLF_H
#define WOLF_H

#include "conn.h"

class Wolf
{
public:
    static Wolf& GetInstance();
    bool CatchGoat();

private:
    const int ALIVE_MAX_DIFF = 70;
    const int DEAD_MAX_DIFF = 20;

    const int DEAD_MOVE_MAX = 2;
    
    Conn conn;
    int cntMove, cntDeadMove, isAlive;

    Wolf();
    Wolf(const Wolf&) = delete;
    Wolf& operator=(const Wolf&) = delete;
};

#endif //WOLF_H
