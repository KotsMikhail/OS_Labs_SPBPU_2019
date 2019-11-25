//
// Created by dmitrii on 23.11.2019.
//

#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <sys/un.h>
#include <conn.h>
#include <message.h>
#include <unistd.h>

bool Conn::is_created;
int Conn::desc;
int Conn::listener;
const char* Conn::channel_name;

Conn::Conn()
{
    channel_name = "/tmp/sock_server";
}

void printErrMsg(const std::string& who, const std::string& msg)
{
    std::cout << "ERROR from " << who << ": " << msg << ", error is: "<< strerror(errno) << std::endl;
}

bool Conn::Open(size_t id, bool create)
{
    std::cout << "start open socket with address: " << channel_name << std::endl;
    struct sockaddr_un saddr{};
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, channel_name);

    is_host = create;
    if (is_created)
    {
        if (is_host)
        {
            desc = accept(listener, NULL, NULL);
            if (desc == -1)
            {
                printErrMsg("host", "fail to accept the socket");
                close(listener);
                unlink(channel_name);
                return false;
            }

        }
        else
        {
            if (connect(desc, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1)
            {
                printErrMsg("client", "fail to connect to socket");
                close(desc);
                return false;
            }
        }

        return true;
    }

    if (create)
    {
        if (unlink(channel_name) == -1)
            std::cout << "WARNING: can't unlink " << channel_name << std::endl;

        // create listener socket
        std::cout << "create listener starting" << std::endl;
        listener = socket(AF_UNIX, SOCK_STREAM, 0);
        if (listener == -1) {
            printErrMsg("host", "fail to create listener");
            return false;
        }
        std::cout << "listener started" << std::endl;

        // bind listener socket
        std::cout << "bind listener starting" << std::endl;
        if (bind(listener, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) < 0)
        {
            printErrMsg("host", "fail to bind listener");
            close(listener);
            return false;
        }
        std::cout << "listener binded" << std::endl;

        std::cout << "listening starting" << std::endl;
        if (listen(listener, 1) == -1)
        {
            printErrMsg("host", "fail to listen");
            close(listener);
            unlink(channel_name);
            return false;
        }
        std::cout << "listening started" << std::endl;

        std::cout << "accept listener starting" << std::endl;
        desc = accept(listener, NULL, NULL);
        if (desc == -1)
        {
            printErrMsg("host", "fail to accept listener");
            close(listener);
            unlink(channel_name);
            return false;
        }
        std::cout << "accept listener started" << std::endl;
    }
    else
    {
        listener = -1;
        desc = socket(AF_UNIX, SOCK_STREAM, 0);
        if (desc == -1)
        {
            printErrMsg("client", "fail to create socket");
            return false;
        }

        if (connect(desc, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1)
        {
            printErrMsg("client", "fail to connect socket");
            close(desc);
            return false;
        }
    }

    is_created = true;
    std::cout << "Socket connection successfully created." << std::endl;
    return true;
}


bool Conn::Close()
{
    if (is_created && close(desc) == 0)
    {
        if (listener != -1)
        {
            if (close(listener) == -1)
            {
                std::cout << "ERROR: Failed to close listener, error: " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (!is_host || (is_host && unlink(channel_name) == 0))
        {
            std::cout << "Connection closed." << std::endl;
            is_created = false;
            return true;
        }

        std::cout << "ERROR: Connection closing failed, error: " << strerror(errno) << std::endl;
    }

    return true;
}


bool Conn::Read(void *buf, size_t count)
{
    if (recv(desc, buf, count, 0) == -1)
    {
        std::cout << "ERROR: failed to read message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}


bool Conn::Write(void *buf, size_t count)
{
    if (send(desc, buf, count, MSG_NOSIGNAL) == -1)
    {
        std::cout << "ERROR: failed to send message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}