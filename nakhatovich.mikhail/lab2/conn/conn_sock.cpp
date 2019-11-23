#include <fcntl.h>
#include <mqueue.h>
#include <syslog.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <new>

#include "connect.h"
#include "message.h"

#define SERVER_PATH "/tmp/lab2_server"

conn_t::conn_t() : _need_to_rm(true), _is_open(false)
{
    _desc = new (std::nothrow) int[2]();
}

conn_t::~conn_t()
{
    delete[] _desc;
}

bool conn_t::conn_open(size_t id, bool create)
{
    if (_desc == nullptr)
        return false;
    
    struct sockaddr_un saddr = {AF_UNIX, SERVER_PATH};
    if (_is_open)
    {
        if (create)
        {
            if ((_desc[1] = accept(_desc[0], NULL, NULL)) == -1)
            {
                syslog(LOG_ERR, "sock: accept failed.");
                close(_desc[0]);
                unlink(SERVER_PATH);
                return (_is_open = false);
            }
            syslog(LOG_ERR, "sock: accepted.");
        }
        else
        {
            if ((connect(_desc[1], (struct sockaddr *)&saddr, SUN_LEN(&saddr))) == -1)
            {
                syslog(LOG_ERR, "sock: connect failed.");
                close(_desc[1]);
                return (_is_open = false);
            }
            syslog(LOG_ERR, "sock: connected.");
        }
        return true;
    }

    _need_to_rm = create;
    
    if (create)
    {
        syslog(LOG_NOTICE, "sock: creating connection with id %lu.", id); 
        _desc[1] = -1;
        if ((_desc[0] = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        {
            syslog(LOG_ERR, "sock: socket failed.");
            return (_is_open = false);
        }
        if ((bind(_desc[0], (struct sockaddr *)&saddr, SUN_LEN(&saddr))) == -1)
        {
            syslog(LOG_ERR, "sock: bind failed.");
            close(_desc[0]);
            return (_is_open = false);
        }
        if ((listen(_desc[0], 1)) == -1)
        {
            syslog(LOG_ERR, "sock: listen failed.");
            close(_desc[0]);
            unlink(SERVER_PATH);
            return (_is_open = false);
        }
        if ((_desc[1] = accept(_desc[0], NULL, NULL)) == -1)
        {
            syslog(LOG_ERR, "sock: accept failed.");
            close(_desc[0]);
            unlink(SERVER_PATH);
            return (_is_open = false);
        }
        syslog(LOG_ERR, "sock: accepted.");
    }
    else
    {
        syslog(LOG_NOTICE, "sock: getting connection with id %lu.", id);
        _desc[0] = -1;
        if ((_desc[1] = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        {
            syslog(LOG_ERR, "sock: socket failed.");
            return (_is_open = false);
        }
        if ((connect(_desc[1], (struct sockaddr *)&saddr, SUN_LEN(&saddr))) == -1)
        {
            syslog(LOG_ERR, "sock: connect failed.");
            close(_desc[1]);
            return (_is_open = false);
        }
        syslog(LOG_ERR, "sock: connected.");
    }

    syslog(LOG_ERR, "sock: opened connection with id %lu.", id);
    return (_is_open = true);
}

bool conn_t::conn_close() 
{
    if ((_is_open || _desc[0] != -1) && (_desc[1] == -1 || !close(_desc[1])) 
        && (_desc[0] == -1 || !close(_desc[0])) 
        && (!_need_to_rm || !unlink(SERVER_PATH))) 
    {
        syslog(LOG_NOTICE, "sock: closed.");
        _is_open = false;
    }
    return !_is_open;
}

bool conn_t::conn_recv(void *buf, size_t count)
{
    if (!_is_open)
    {
        syslog(LOG_ERR, "sock: couldn't read data.");
        return false;
    }
    if (recv(_desc[1], buf, count, 0) == -1)
    {
        syslog(LOG_ERR, "sock: recv failed.");
        return false;
    }
    syslog(LOG_NOTICE, "sock: read data.");
    return true;
}

bool conn_t::conn_send(void *buf, size_t count)
{
    if (!_is_open)
    {
        syslog(LOG_ERR, "sock: couldn't write data.");
        return false;
    }
    if (send(_desc[1], buf, count, MSG_NOSIGNAL) == -1) 
    {
        syslog(LOG_ERR, "sock: send failed.");
        return false;
    }
    syslog(LOG_NOTICE, "sock: wrote data.");
    return true;
}
