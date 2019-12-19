#include "conn.h"
#include "Message.h"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <map>
#include <unistd.h>


bool conn::Read(void *buf, size_t count) {
    if (read(fd[0], buf, count) != -1){
        return true;
    }
    std::cout << "Error on reading message" << std::endl;

    return false;
}

bool conn::Write(void *buf, size_t count) {
    if (write(fd[1], buf, count) != -1){
        return true;
    }
    std::cout << "Error on writing message" << std::endl;
    return false;
}

bool conn::Open(size_t id) {
    fd = new int[2];
    if (fd != nullptr) {
        if (pipe(fd) != -1){
            return true;
        }
    }
    std::cout << "Error on opening connection for client " << id << std::endl;
    return false;
}

void conn::Close(){
    close(fd[0]);
    close(fd[1]);
}