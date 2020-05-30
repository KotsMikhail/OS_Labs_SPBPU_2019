#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstdlib>
#include <string>

#include "conn.h"

using namespace std;

const string FIFO_PATH_NAME = "fifo";

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
        if (mkfifo(FIFO_PATH_NAME.c_str(), S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH) == -1)
            ReportError("Mkfifo error.");
    
    fileDesc[0] = open(FIFO_PATH_NAME.c_str(), O_RDWR);
    
    if (fileDesc[0] == -1)
        ReportError("Open error.");
    
    if (create)
        syslog(LOG_INFO, "Host opened a connection.");
    else
        syslog(LOG_INFO, "Client connected to connection.");
}

void Conn::Write(int num)
{
    if (write(fileDesc[0], &num, sizeof(int)) == -1)
        ReportError("Write error.");
}

int Conn::Read()
{
    int num;

    if (read(fileDesc[0], &num, sizeof(int)) == -1)
        ReportError("Read error.");
    
    return num;
}

Conn::~Conn()
{
    if (create)
    {
        if (close(fileDesc[0]) == -1)
            ReportError("Close error.");
    
        if (unlink(FIFO_PATH_NAME.c_str()) == -1)
            ReportError("Unlink error.");
        
        syslog(LOG_INFO, "Host closed a connection");
        syslog(LOG_INFO, "Host completed.");
    }
}
