#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <cstdlib>
#include <string>

#include "conn.h"

using namespace std;

const string SOCK_PATH = "sock";

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

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH.c_str(), sizeof(addr.sun_path) - 1);

    if (create)
    {
        fileDesc[0] = socket(AF_UNIX, SOCK_STREAM, 0);

        if (fileDesc[0] == -1)
            ReportError("Socket error.");
        
        if (bind(fileDesc[0], (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
            ReportError("Bind error.");

        if (listen(fileDesc[0], 1) == -1)
            ReportError("Listen error.");
        
        fileDesc[1] = accept(fileDesc[0], nullptr, nullptr);

        if (fileDesc[1] == -1)
            ReportError("Accept error.");
        
        syslog(LOG_INFO, "Host opened a connection.");
    }
    else
    {
        fileDesc[1] = socket(AF_UNIX, SOCK_STREAM, 0);

        if (fileDesc[1] == -1)
            ReportError("Socket error.");

        if (connect(fileDesc[1], (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
            ReportError("Connect error.");

        syslog(LOG_INFO, "Client connected to connection.");     
    }
}

void Conn::Write(int num)
{
    if (send(fileDesc[1], &num, sizeof(int), MSG_NOSIGNAL) == -1)
        ReportError("Send error.");
}

int Conn::Read()
{
    int num;

    if (recv(fileDesc[1], &num, sizeof(int), 0) == -1)
        ReportError("Recv error.");
    
    return num;
}

Conn::~Conn()
{
    if (create)
    {
        if (close(fileDesc[1]) == -1)
            ReportError("Close error.");

        if (close(fileDesc[0]) == -1)
            ReportError("Close error.");
    
        if (unlink(SOCK_PATH.c_str()) == -1)
            ReportError("Unlink error.");
        
        syslog(LOG_INFO, "Host closed a connection");
        syslog(LOG_INFO, "Host completed.");
    }
}
