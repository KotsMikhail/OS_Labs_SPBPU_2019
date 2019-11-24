#include <conn.h>
#include <message.h>

#include <iostream>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstring>
#include <string>
#include <mqueue.h>
#include <cerrno>

#define QUEUE_NAME "/LAB2_QUEUE"
#define QUEUE_MAXMSG 1
#define QUEUE_MSGSIZE sizeof(Message)
#define QUEUE_ATTR_INITIALIZER ((struct mq_attr){0, QUEUE_MAXMSG, QUEUE_MSGSIZE, 0, {0}})


bool Conn::Open(size_t id, bool create)
{
    bool res = false;
    _owner = create;
    int mq_flg = O_RDWR;
    int mq_perm = 0666;
    if (_owner)
    {
        std::cout << "Creating connection with id: " << id << std::endl;
        mq_flg |= O_CREAT;
        struct mq_attr attr = QUEUE_ATTR_INITIALIZER;
        _id = mq_open(QUEUE_NAME, mq_flg, mq_perm, &attr);
    }
    else
    {
        std::cout << "Getting connection with id: " << id << std::endl;
        _id = mq_open(QUEUE_NAME, mq_flg);
    }
    if (_id == -1)
    {
        std::cout << "ERROR: mq_open failed, errno: " << strerror(errno) << std::endl;
    }
    else
    {
        res = true;
    }
    return res;
}

bool Conn::Read(void* buf, size_t count)
{
    Message mq_buf;
    bool success = false;
    if (count <= QUEUE_MSGSIZE)
    {
        if (mq_receive(_id, (char *) &mq_buf, QUEUE_MSGSIZE, nullptr) == -1)
        {
            std::cout << "ERROR: mq_recieve failed, errno: " << strerror(errno) << std::endl;
        } else
        {
            success = true;
            memcpy(buf, (void*)&mq_buf, count);
        }
    }
    return success;
}

bool Conn::Write(void* buf, size_t count)
{
    bool success = false;
    if (count <= QUEUE_MSGSIZE)
    {
        if (mq_send(_id, (char*) buf, count, 0) == -1)
        {
            std::cout << "ERROR: mq_send failed, errno: " << strerror(errno) << std::endl;
        }
        else
        {
            success = true;
        }
    }
    return success;
}

bool Conn::Close()
{
    bool res = false;
    if (mq_close(_id) == 0)
    {
        if (!_owner || (_owner && mq_unlink(QUEUE_NAME) == 0))
        {
            res = true;
        }
    }
    return res;
}
