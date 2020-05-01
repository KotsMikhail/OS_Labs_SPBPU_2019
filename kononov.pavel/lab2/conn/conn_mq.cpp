#include <conn.h>
#include <message.h>
#include "../utils/utils.h"

#include <iostream>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <cerrno>


Conn::Conn () {
}

Conn::~Conn () {
}

bool Conn::Open(size_t id, bool create) {
    m_owner = create;
    m_name = "/LAB2_QUEUE";
    m_id = id;
    int mq_flg = O_RDWR;
    int mq_perm = 0666;
    if (m_owner) {
        std::cout << "Creating connection with id: " << id << std::endl;
        mq_flg |= O_CREAT;
        struct mq_attr attr = ((struct mq_attr) {0, 1, sizeof(Message), 0, {0}});
        m_desc = mq_open(GetName(m_name.c_str(), m_id).c_str(), mq_flg, mq_perm, &attr);
    } else {
        std::cout << "Getting connection with id: " << id << std::endl;
        m_desc = mq_open(GetName(m_name.c_str(), m_id).c_str(), mq_flg);
    }
    if (m_desc == -1) {
        std::cout << "ERROR: mq_open failed, errno: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::Read(void* buf, size_t count) {
    if (mq_receive(m_desc, (char *) buf, count, nullptr) == -1) {
        std::cout << "ERROR: mq_recieve failed, errno: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::Write(void* buf, size_t count) {
    if (mq_send(m_desc, (char *) buf, count, 0) == -1) {
        std::cout << "ERROR: mq_send failed, errno: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::Close() {
    if (mq_close(m_desc) == 0) {
        if (!m_owner || (mq_unlink(GetName(m_name.c_str(), m_id).c_str()) == 0)) {
            return true;
        }
    }
    return false;
}
