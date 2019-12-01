#include <iostream>

//#define LAB_2_SEVER__CONN_SHM_MEM_H
#define LAB_2_SEVER__CONN_PIPE_H
#define LAB_2_SEVER__CONN_MQ_H
#define LAB_2_SEVER__CONN_FIFO_H

#include <syslog.h>
#include <unistd.h>
#include "Server.h"


int main() {

    openlog("lab2", LOG_NOWAIT | LOG_PID, LOG_LOCAL1);
    Server *server = Server::GetInstance();
    syslog(LOG_NOTICE, "host: started with pid %d.", getpid());
    server->Start();
    syslog(LOG_NOTICE, "host: stopped.");
    closelog();

    return 0;
}