//
// Created by dmitrii on 23.11.2019.
//

#include <conn.h>
#include <message.h>
#include <errno.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <zconf.h>

int Conn::desc;
const char* Conn::channel_name;

Conn::Conn()
{
    channel_name = "/tmp/fifo_pipe";
}

bool Conn::Open(size_t id, bool create)
{
    bool res = false;
    is_host = create;
    int fifoflg = 0777;

    if (create)
    {
        std::cout << "Creating connection with id = " << id << std::endl;
        unlink(channel_name);
    }
    else
    {
        std::cout << "Getting connection with id = " << id << std::endl;
    }

    if (is_host && mkfifo(channel_name, fifoflg) == -1)
    {
        std::cout << "ERROR: mkfifo failed, error = " << strerror(errno) << std::endl;
    }
    else
    {
        res = true;
    }
    return res;
}

bool Conn::Read(void* buf, size_t count)
{
    Message shm_buf;
    bool success = false;
    if ((desc = open(channel_name, O_RDONLY)) == -1)
    {
        std::cout << "ERROR: can't open fifo pipe for reading, error = " << strerror(errno) << std::endl;
    }
    else
    {
        if (read(desc, &shm_buf, count) == -1)
        {
            std::cout << "ERROR: reading failed with error = " << strerror(errno) << std::endl;
        }
        else
        {
            *((Message*) buf) = shm_buf;
            success = true;
        }
        close(desc);
    }
    return success;
}

bool Conn::Write(void* buf, size_t count)
{
    bool success = false;
    if ((desc = open(channel_name, O_WRONLY)) == -1)
    {
        std::cout << "ERROR: can't open pipe for writing, error = " << strerror(errno) << std::endl;
    }
    else
    {
        if (write(desc, buf, count) == -1)
        {
            std::cout << "ERROR: writing failed with error = " << strerror(errno) << std::endl;
        }
        else
        {
            success = true;
        }
        close(desc);
    }
    return success;
}

bool Conn::Close()
{
    bool res = true;
    if (is_host && remove(channel_name) < 0)
    {
        res = false;
    }
    return res;
}
