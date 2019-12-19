//
// Created by nero on 16.11.2019.
//

#include "conn.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <map>

const off_t length = 1024;

bool conn::Read(void *buf, size_t count)
{
    memcpy(buf, fd, count);
    return true;
}
bool conn::Write(void *buf, size_t count)
{
    memcpy(fd, buf, count);
    return true;
}
bool conn::Open(size_t id)
{
    fd = new int;
    if (fd != nullptr) {
        name = "shm_file" + std::to_string(id);
        int shm_id = shm_open(name.c_str(), O_RDWR | O_CREAT, 0666);
        if (shm_id != -1) {
            ftruncate(shm_id, length);
            fd = (int *) mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
            if (fd != (int*)-1){
                return true;
            }
        }
    }
    std::cout << "Error on opening connection for client " << id << std::endl;
    return false;

}
void conn::Close() {
    munmap(fd, length);
    shm_unlink(name.c_str());
}