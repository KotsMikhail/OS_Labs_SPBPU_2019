#ifndef OS_LABS_SPBPU_2019_WOLF_H
#define OS_LABS_SPBPU_2019_WOLF_H

#include <csignal>
#include <member.h>
#include "client_info.h"

class Wolf: public Member
{
public:
    void Start();
    bool OpenConnection();

    static Wolf& GetInstance();

private:
    ClientInfo client_info;
    int curr_num;
    static const int RAND_LIMIT = 100;

    void Terminate(int signum);

    Wolf();
    Wolf(const Wolf&);
    Message Step(Message& msg);
    static void SignalHandler(int signum, siginfo_t* info, void *ptr);
};

#endif //OS_LABS_SPBPU_2019_WOLF_H
