#include <iostream>
#include <cstdlib>

#include "wolf.h"

const int STR_LEN_MAX = 1000;

Wolf::Wolf()
    : conn(getpid(), true), cntMove(1), cntDeadMove(0), isAlive(1)
{
    printf("Game on\n");
}

Wolf& Wolf::GetInstance()
{
    static Wolf instance;
    return instance;
}

bool Wolf::CatchLittleGoat()
{
    printf("Move: %d\n", cntMove);

    int littleGoatNum = conn.Read();

    if (isAlive)
        printf("Alive little goat: %d\n", littleGoatNum);
    else
        printf("Dead little goat: %d\n", littleGoatNum);
    
    char str[STR_LEN_MAX];
    printf("Wolf: ");
    while (fgets(str, STR_LEN_MAX, stdin) == nullptr || atoi(str) < 1 || atoi(str) > 100)
    {
        printf("Wrong wolf number\n");
        printf("Wolf: ");
    }

    int wolfNum = atoi(str);
    int diff = abs(wolfNum - littleGoatNum);

    if (isAlive)
    {
        if (diff <= ALIVE_MAX_DIFF)
            printf("Diff = |%d - %d| = %d <= %d => Alive\n", wolfNum, littleGoatNum, diff, ALIVE_MAX_DIFF);
        else
        {
            isAlive = 0;
            printf("Diff = |%d - %d| = %d  > %d => Dead\n", wolfNum, littleGoatNum, diff, ALIVE_MAX_DIFF);
        }
    }
    else
    {
        if (diff <= DEAD_MAX_DIFF)
        {
            printf("Diff = |%d - %d| = %d <= %d => Alive\n", wolfNum, littleGoatNum, diff, DEAD_MAX_DIFF);
            isAlive = 1;
            cntDeadMove = 0;
        }
        else
        {
            printf("Diff = |%d - %d| = %d > %d => Dead\n", wolfNum, littleGoatNum, diff, DEAD_MAX_DIFF);
            cntDeadMove++;

            if (cntDeadMove == 2)
            {
                printf("2 dead little goat's moves in a row => Game over\n");

                return true;
            }
        }
    }

    conn.Write(isAlive);

    cntMove++;

    return false;
}
