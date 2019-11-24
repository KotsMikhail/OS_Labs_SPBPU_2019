#ifndef OS_LABS_SPBPU_2019_GOAT_H
#define OS_LABS_SPBPU_2019_GOAT_H

#include <member.h>

class Goat: public Member
{
public:
    void Start();
    bool OpenConnection();
    static Goat& GetInstance(int host_pid);
private:
    int host_pid;
    const int RAND_LIMIT_ALIVE = 100;
    const int RAND_LIMIT_DEAD = 50;

    void Terminate(int signum);

    Goat(int host_pid);
    Goat(const Goat&);
    static void SignalHandler(int signum);
};

#endif //OS_LABS_SPBPU_2019_GOAT_H
