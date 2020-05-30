#include <syslog.h>
#include <signal.h>
#include <mqueue.h>

#include <cstdlib>
#include <string>

#include "conn.h"

using namespace std;

const string MQ_NAME = "/mq";

void Conn::ReportError(const string &msg)
{
    syslog(LOG_ERR, "%s", msg.c_str());
    kill(pid, SIGTERM);
    exit(EXIT_FAILURE);
}

Conn::Conn(pid_t p, bool c)
{
    pid = p;
    create = c;

    if (create)
    {
        struct mq_attr mqa = { 0, 1, sizeof(int), 0 };
        fileDesc[0] = mq_open(MQ_NAME.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH, &mqa);
    }
    else
        fileDesc[0] = mq_open(MQ_NAME.c_str(), O_RDWR);
    
    if (fileDesc[0] == -1)
        ReportError("Mq_open error.");
    
    if (create)
        syslog(LOG_INFO, "Host opened a connection.");
    else
        syslog(LOG_INFO, "Client connected to connection.");
}

void Conn::Write(int num)
{
    if (mq_send(fileDesc[0], (const char*)(&num), sizeof(int), 0) == -1)
        ReportError("Mq_send error.");
}

int Conn::Read()
{
    int num;

    if (mq_receive(fileDesc[0], (char*)(&num), sizeof(int), nullptr) == -1)
        ReportError("Mq_receive error.");
    
    return num;
}

Conn::~Conn()
{
    if (create)
    {
        if (mq_close(fileDesc[0]) == -1)
            ReportError("Mq_close error.");
    
        if (mq_unlink(MQ_NAME.c_str()) == -1)
            ReportError("Mq_unlink error.");
    
        syslog(LOG_INFO, "Host closed a connection.");
        syslog(LOG_INFO, "Host completed.");
    }
}
