#include "conn.h"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <map>
#include <unistd.h>



void conn::Read(void *buf, size_t count) {
    read(fd[0], buf, count);
}

void conn::Write(void *buf, size_t count) {
    write(fd[1], buf, count);
}

void conn::Open(size_t id) {
    fd = (int *) malloc(2 * sizeof(int));
    int pipefd[2];
    pipe(pipefd);
    fd[0] = pipefd[0];
    fd[1] = pipefd[1];
}

void conn::Close() {
    close(fd[0]);
    close(fd[1]);
}