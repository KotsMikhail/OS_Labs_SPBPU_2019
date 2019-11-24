//
// Created by dmitrii on 23.11.2019.
//

#include <conn.h>
#include <message.h>
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>

const static char* QUEUE_NAME = "/mq_queue";
const static int QUEUE_MAXMSG = 1;
const static int QUEUE_MSGSIZE = sizeof(Message);
const static mq_attr QUEUE_ATTR_INITIALIZER = ((struct mq_attr){0, QUEUE_MAXMSG, QUEUE_MSGSIZE, 0, {0}});

mqd_t mqid;
bool g_is_host;

bool Conn::Open(size_t id, bool create)
{
    bool res = false;
    g_is_host = create;
    int mqflg = O_RDWR;
    int mqperm = 0666;

    if (g_is_host)
    {
        std::cout << "Creating connection with id = " << id << std::endl;
        mqflg |= O_CREAT;
        struct mq_attr attr = QUEUE_ATTR_INITIALIZER;
        mqid = mq_open(QUEUE_NAME, mqflg, mqperm, &attr);
    }
    else
    {
        std::cout << "Getting connection with id = " << id << std::endl;
        mqid = mq_open(QUEUE_NAME, mqflg);
    }

    if (mqid == -1)
    {
        std::cout << "ERROR: mq_open failed, errno = " << strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "mq_open returned id = " << mqid << std::endl;
        res = true;
    }
    return res;
}

bool Conn::Read(void* buf, size_t count)
{
    Message mq_buf;
    bool success = true;
    if (mq_receive(mqid, (char *)&mq_buf, QUEUE_MSGSIZE, nullptr) == -1)
    {
        std::cout << "ERROR: mq_recieve failed, errno = " << strerror(errno) << std::endl;
        success = false;
    }
    else
    {
        *((Message*) buf) = mq_buf;
    }
    return success;
}

bool Conn::Write(void* buf, size_t count)
{
    bool res = false;
    if (count <= QUEUE_MSGSIZE)
    {
        if (mq_send(mqid, (char*)buf, count, 0) == -1)
        {
            std::cout << "ERROR: mq_send failed, errno = " << strerror(errno) << std::endl;
        }
        else
        {
            res = true;
        }
    }
    return res;
}

bool Conn::Close()
{
    bool res = false;
    if (mq_close(mqid) == 0)
    {
        if (!g_is_host)
        {
            res = true;
        }
        else if (g_is_host && mq_unlink(QUEUE_NAME) == 0)
        {
            res = true;
        }
    }
    return res;
}


