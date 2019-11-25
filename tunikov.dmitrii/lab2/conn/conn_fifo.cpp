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


Conn::Conn() : is_open(false)
{
    desc_ = new (std::nothrow) int (-1);
    channel_name = "/tmp/fifo_pipe1";
}

Conn::~Conn()
{
    delete (int*)desc_;
}

bool Conn::Open(size_t id, bool create)
{
    int* desc = (int*)desc_;

    if (desc == nullptr)
        return false;

    if (is_open)
        return true;

    is_host = create;

    if (create)
    {
        unlink(channel_name);
        if (mkfifo(channel_name, 0666) == -1)
        {
            return (is_open = false);
        }
    }

    *desc = open(channel_name, O_RDWR);

    is_open = (*desc != -1);
    if (is_open)
        std::cout << "fifo open connection with id " << id << std::endl;
    else
    {
        std::cout << "fifo get connection with id " << id << std::endl;
        if (is_host)
            unlink(channel_name);
    }

    return is_open;
}

bool Conn::Read(void* buf, size_t count)
{
    if (!is_open)
    {
        std::cout << "ERROR: can't read bacause fifo not opened" << std::endl;
        return false;
    }

    int* desc = (int*)desc_;
    if (read(*desc, buf, count) == -1)
    {
        std::cout << "ERROR: reading failed with error = " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::Write(void* buf, size_t count)
{
    if (!is_open)
    {
        std::cout << "ERROR: can't write bacause fifo not opened" << std::endl;
        return false;
    }

    int* desc = (int*)desc_;
    if (write(*desc, buf, count) == -1)
    {
        std::cout << "ERROR: writing failed with error = " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::Close()
{
    int* desc = (int*)desc_;
    if (is_open && !close(*desc) && (!is_host || !unlink(channel_name)))
    {
        std::cout << "fifo closed" << std::endl;
        is_open = false;
    }

    return !is_open;
}