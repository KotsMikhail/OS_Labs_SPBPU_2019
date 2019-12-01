#include <iostream>
#include <sys/un.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <new>

#include <conn.h>
#include <message.h>

const char * const FIFO_PATH = "/tmp/lab2_fifo_path";


struct ConnFifoArgs
{
    bool wasCreated;
};


Conn::Conn ()
{
    pmem = new (std::nothrow) ConnFifoArgs();
}


Conn::~Conn ()
{
    delete (ConnFifoArgs *)pmem;
}


bool Conn::Open (size_t id, bool create)
{
    ConnFifoArgs *fifoArgs = (ConnFifoArgs *)pmem;
    if (fifoArgs == NULL) {
        std::cout << "[ERROR]: FIFO connection's arguments invalid." << std::endl;
        return false;
    }

    if (fifoArgs->wasCreated) {
        return true;
    }

    isHost = create;
    if (create) {
        std::cout << "Creating fifo..." << std::endl;
        if (mkfifo(FIFO_PATH, 0666) == -1) {
            std::cout << "[ERROR]: Fifo creating failed, error: " << strerror(errno) << std::endl;
            return false;
        }
    } else {
        std::cout << "Getting fifo..." << std::endl;
    }

    descID = open(FIFO_PATH, O_RDWR);
    if (descID == -1) {
        std::cout << "[ERROR]: Failed to open fifo, error: " << strerror(errno) << std::endl;
        if (isHost) {
            unlink(FIFO_PATH);
        }

        return false;
    }
    std::cout << "Fifo succeeded." << std::endl;

    fifoArgs->wasCreated = true;
    return true;
}


bool Conn::Close ()
{
    ConnFifoArgs *fifoArgs = (ConnFifoArgs *)pmem;
    if (fifoArgs == NULL) {
        std::cout << "[ERROR]: FIFO connection's arguments invalid." << std::endl;
        return false;
    }

    if (fifoArgs->wasCreated) {
        if (close(descID) == -1) {
            std::cout << "[ERROR]: Connection closing failed, error: " << strerror(errno) << std::endl;
            return false;
        }

        if (!isHost || (isHost && unlink(FIFO_PATH) == 0)) {
            std::cout << "Connection closed." << std::endl;
            fifoArgs->wasCreated = false;
            return true;
        }
    }

    return true;
}


bool Conn::Read (void *buf, size_t count)
{
    if (read(descID, buf, count) == -1) {
        std::cout << "[ERROR]: failed to read message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}


bool Conn::Write (void *buf, size_t count)
{
    if (write(descID, buf, count) == -1) {
        std::cout << "[ERROR]: failed to send message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}
