#include "conn.h"
#include "Message.h"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <map>
#include <unistd.h>


std::map<int, int *> pipes;

void conn::Read(size_t id, void *buf, size_t count) {
    read(pipes[id][0], buf, count);
}

void conn::Write(size_t id, void *buf, size_t count) {
    write(pipes[id][1], buf, count);
}

void conn::Open(size_t id) {
    int *fd = (int *) malloc(2 * sizeof(int));
    int pipefd[2];
    pipe(pipefd);
    fd[0] = pipefd[0];
    fd[1] = pipefd[1];
    pipes.insert(std::pair<int, int *>(id, fd));
}

void conn::Close(size_t id){
    close(pipes[id][0]);
    close(pipes[id][1]);
}