#ifndef OS_LABS_SPBPU_2019_MEMBER_H
#define OS_LABS_SPBPU_2019_MEMBER_H

#include <conn.h>
#include <semaphore.h>
#include <message.h>

class Member
{
public:
    virtual void Start() = 0;
    virtual bool OpenConnection() = 0;

private:
    virtual void Terminate(int signum) = 0;
    virtual bool CheckSelfMessage(Message& msg) = 0;

protected:
    Conn connection;
    sem_t* semaphore;
    Member() {}
    ~Member() {}
};

#endif //OS_LABS_SPBPU_2019_MEMBER_H
