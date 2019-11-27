#include <cstdlib>
#include <ctime>

#include "little_goat.h"

LittleGoat::LittleGoat()
    : conn(getppid(), false), isAlive(1)
{
    srand(time(nullptr));
}

LittleGoat& LittleGoat::GetInstance()
{
    static LittleGoat instance;
    return instance;
}

void LittleGoat::MakeMove()
{
    int littleGoatNum;

    if (isAlive)
        littleGoatNum = rand() % ALIVE_MAX + 1;
    else
        littleGoatNum = rand() % DEAD_MAX + 1;

    conn.Write(littleGoatNum);
}

void LittleGoat::FindOutStatus()
{
    isAlive = conn.Read();
}
