#ifndef OS_LABS_SPBPU_2019_WOLF_H
#define OS_LABS_SPBPU_2019_WOLF_H

#include <csignal>
#include <member.h>
#include "client_info.h"

#define RAND_LIMIT_WOLF 100

class Wolf: public Member
{
public:
    void Start();
    bool OpenConnection();

    static Wolf& GetInstance();

private:
    ClientInfo client_info;
    int curr_num;

    void Terminate(int signum);
    bool CheckSelfMessage(Message& msg);

    Wolf();
    Message Step(Message& msg);
    static void SignalHandler(int signum, siginfo_t* info, void *ptr);
};

#endif //OS_LABS_SPBPU_2019_WOLF_H
