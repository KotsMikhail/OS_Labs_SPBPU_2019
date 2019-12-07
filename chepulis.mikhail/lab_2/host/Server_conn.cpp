//
// Created by misha on 26.11.2019.
//




#include "Server_conn.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <syslog.h>



bool Server_conn::Create() {
    owner = true;
    is_closed = false;
    if (pipe(fd) == -1) {
        perror("pipe");
        //std::cout << "ERROR: pipe failed, error = " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "ERROR: pipe failed, error = %s", strerror(errno));
        return false;
    }

    return true;
}

bool Server_conn::Open(size_t id) {
    client_pid = id;
    return true;
}

bool Server_conn::Read(void *buf, size_t count) {
    if (read(fd[0], buf, count) <= 0) {
        //std::cout << "ERROR: reading failed with error = " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "ERROR: reading failed with error = %s", strerror(errno));
        return false;
    }
    return true;
}


bool Server_conn::Write(void *buf, size_t count) {
    if (write(fd[1], buf, count) == -1) {
        //std::cout << "ERROR: writing failed with error = " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "ERROR: writing failed with error = %s", strerror(errno));
        return false;
    }
    return true;
}

bool Server_conn::Close() {
    if (owner) {
        if ((close(fd[0]) < 0) && (close(fd[1]) < 0)) {
            std::cout << "ERROR: can`t close = " << strerror(errno) << std::endl;
            return false;
        }
        is_closed = true;
    }
    return true;
}

int Server_conn::GetClientID() {
    return client_pid;
}

bool Server_conn::IsClosed() {
    return is_closed;
}




