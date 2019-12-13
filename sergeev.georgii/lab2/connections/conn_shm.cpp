//
// Created by nero on 16.11.2019.
//

#include "conn.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <map>


std::map<int, char*> addresses;
std::map<int, const char*> names;
const off_t length = 1024;

void conn::Read(size_t id, void *buf, size_t count)
{
    memcpy(buf, addresses[id], count);
}
void conn::Write(size_t id, void *buf, size_t count)
{
    memcpy(addresses[id], buf, count);
}
void conn::Open(size_t id)
{
    std::string name = "shm_file" + std::to_string(id);
    int fd = shm_open(name.c_str(),  O_RDWR | O_CREAT,  0666);
    ftruncate(fd, length);
    char* addr = (char *) mmap(nullptr, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    addresses.insert(std::pair<int, char*>(id, addr));
    const char* name_c = name.c_str();
    names.insert(std::pair<int, const char*>(id, name_c));
}
void conn::Close(size_t id) {
    munmap(addresses[id], length);
    shm_unlink(names[id]);
}