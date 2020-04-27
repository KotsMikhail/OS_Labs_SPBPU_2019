#include <conn.h>

#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <message.h>
#include <cerrno>
#include "../utils/utils.h"

Conn::Conn () {
}

Conn::~Conn () {
}

bool Conn::Open(size_t id, bool create) {
    m_owner = create;
    m_name = "/tmp/lab2_fifo";
    m_id = id;

    if (m_owner && mkfifo(GetName(m_name.c_str(), m_id).c_str(), 0777) == -1) {
        std::cout << "ERROR: mkfifo failed: " << strerror(errno) << std::endl;
        return false;
    } else {
        m_desc = open(GetName(m_name.c_str(), m_id).c_str(), O_RDWR);
        if (m_desc == -1) {
            std::cout << "ERROR: open failed: " << strerror(errno) << std::endl;
            return false;
        }
    }
    return true;
}

bool Conn::Read(void* buf, size_t count) {
    if (read(m_desc, buf, count) == -1) {
        std::cout << "ERROR: reading failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::Write(void* buf, size_t count) {
    if (write(m_desc, buf, count) == -1) {
        std::cout << "ERROR: writing failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::Close() {
    if (close(m_desc) < 0 || (m_owner && remove(GetName(m_name.c_str(), m_id).c_str()) < 0)) {
        std::cout << "ERROR: close failed: " << strerror(errno) << std::endl;
        std::cout << GetName(m_name.c_str(), m_id).c_str() << std::endl;
        return false;
    }
    return true;
}
