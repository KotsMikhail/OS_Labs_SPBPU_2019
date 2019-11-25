#include <iostream>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>

#include <conn.h>
#include <message.h>

const char * const QUEUE_NAME = "/lab2_message_queue";
const int          QUEUE_MAXMSG = 1;
const size_t       QUEUE_MSGSIZE = sizeof(Message);


Conn::Conn ()
{
    additionalArgs = NULL;
}


Conn::~Conn ()
{
}


bool Conn::Open (size_t id, bool create)
{
    isHost = create;

    int flg = O_RDWR;

    if (create) {
        std::cout << "Creating message queue with id: " << id << std::endl;

        struct mq_attr attr;
        memset(&attr, 0, sizeof(attr));
        attr.mq_maxmsg = QUEUE_MAXMSG;
        attr.mq_msgsize = QUEUE_MSGSIZE;

        descID = mq_open(QUEUE_NAME, flg | O_CREAT, 0666, &attr);
    } else {
        std::cout << "Getting message queue by id: " << id << std::endl;
        descID = mq_open(QUEUE_NAME, flg);
    }

    if (descID == -1) {
        std::cout << "[ERROR]: mq_open failed with error: " << strerror(errno) << std::endl;
        return false;
    }

    std::cout << "mq_open succeeded, mqID: " << descID << std::endl;
    return true;
}


bool Conn::Close ()
{
    if (mq_close(descID) == 0) {
        if (!isHost) {
            std::cout << "Connection closed." << std::endl;
            return true;
        } else if (mq_unlink(QUEUE_NAME) == 0) {
            std::cout << "Connection closed." << std::endl;
            return true;
        }
    }

    std::cout << "[ERROR]: Failed to close connection, error: " << strerror(errno) << std::endl;
    return false;
}


bool Conn::Read (void *buf, size_t count)
{
    Message msg;
    if (mq_receive(descID, (char *)&msg, QUEUE_MSGSIZE, NULL) == -1) {
        std::cout << "[ERROR] mq_receive failed with error: " << strerror(errno) << std::endl;
        return false;
    }

    *((Message *)buf) = msg;
    return true;
}


bool Conn::Write (void *buf, size_t count)
{
    if (count > QUEUE_MSGSIZE) {
        std::cout << "[ERROR]: size of message is big, get: " << count << ", expected max: " << QUEUE_MSGSIZE << std::endl;
        return false;
    }

    if (mq_send(descID, (char *)buf, count, 0) == -1) {
        std::cout << "[ERROR]: mq_send failed with error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}