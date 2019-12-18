#include "conn.h"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <map>
#include <unistd.h>


bool conn::Read(void *buf, size_t count) {
    if (read(fd[0], buf, count) <= 0) {
        std::cout << "ERROR: reading failed with error = " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool conn::Write(void *buf, size_t count) {
    if (write(fd[1], buf, count) == -1) {
        std::cout << "ERROR: writing failed with error = " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool conn::Open(size_t id) {

    fd = new int[2];
    if (fd == nullptr) {
        return false;
    }
    if (pipe(fd) == -1) {
        std::cout << "ERROR: pipe failed with error = " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

void conn::Close() {
    close(fd[0]);
    close(fd[1]);
}