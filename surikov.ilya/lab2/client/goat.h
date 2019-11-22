#ifndef OS_LABS_SPBPU_2019_GOAT_H
#define OS_LABS_SPBPU_2019_GOAT_H

#include <member.h>

#define RAND_LIMIT_ALIVE 100
#define RAND_LIMIT_DEAD 50

class Goat: public Member
{
public:
    void Start();
    bool OpenConnection();
    static Goat& GetInstance(int host_pid);
private:
    int host_pid;

    void Terminate(int signum);
    bool CheckSelfMessage(Message& msg);

    Goat(int host_pid);
    static void SignalHandler(int signum);
};

#endif //OS_LABS_SPBPU_2019_GOAT_H
