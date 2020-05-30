#include <cstdlib>
#include <ctime>

#include "goat.h"

Goat::Goat()
    : conn(getppid(), false), isAlive(1)
{
    srand(time(nullptr));
}

Goat& Goat::GetInstance()
{
    static Goat instance;
    return instance;
}

void Goat::MakeMove()
{
    int goatNum;

    if (isAlive)
        goatNum = rand() % ALIVE_MAX + 1;
    else
        goatNum = rand() % DEAD_MAX + 1;

    conn.Write(goatNum);
}

void Goat::FindOutStatus()
{
    isAlive = conn.Read();
}
