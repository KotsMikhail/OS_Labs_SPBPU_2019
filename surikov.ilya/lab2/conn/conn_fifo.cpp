#include <conn.h>

#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <message.h>
#include <cerrno>

const char* FIFO_NAME = "/tmp/lab2_fifo";


bool Conn::Open(size_t id, bool create)
{
    bool res = false;
    _owner = create;
    int fifo_flg = 0777;
    if (create)
    {
        std::cout << "Creating connection with id: " << id << std::endl;
    } else {
        std::cout << "Getting connection with id: " << id << std::endl;
    }
    if (_owner && mkfifo(FIFO_NAME, fifo_flg) == -1)
    {
        std::cout << "ERROR: mkfifo failed, error: " << strerror(errno) << std::endl;
    } else {
        res = true;
    }
    return res;
}

bool Conn::Read(void* buf, size_t count)
{
    Message fifo_buf;
    bool success = false;
    if ((_id = open(FIFO_NAME, O_RDONLY)) == -1)
    {
        std::cout << "ERROR: can't open for reading, error:" << strerror(errno) << std::endl;
    } else {
        if (read(_id, &fifo_buf, count) == -1)
        {
            std::cout << "ERROR: reading failed with error: " << strerror(errno) << std::endl;
        } else {
            memcpy(buf, (void*)&fifo_buf, count);
            success = true;
        }
        close(_id);
    }
    return success;
}

bool Conn::Write(void* buf, size_t count)
{
    bool success = false;
    if ((_id = open(FIFO_NAME, O_WRONLY)) == -1)
    {
        std::cout << "ERROR: can't open pipe for writing, error: " << strerror(errno) << std::endl;
    } else {
        if (write(_id, buf, count) == -1)
        {
            std::cout << "ERROR: writing failed with error: " << strerror(errno) << std::endl;
        } else {
            success = true;
        }
        close(_id);
    }
    return success;
}

bool Conn::Close()
{
    bool res = true;
    if (_owner && remove(FIFO_NAME) < 0)
    {
        res = false;
    }
    return res;
}
