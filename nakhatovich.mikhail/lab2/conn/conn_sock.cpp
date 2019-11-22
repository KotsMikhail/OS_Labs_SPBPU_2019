#include <fcntl.h>
#include <mqueue.h>
#include <syslog.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "connect.h"
#include "message.h"

#define SERVER_PATH "/tmp/lab2_server"

bool _need_to_rm = true, _is_open = false;
int _sd1 = -1, _sd2 = -1;

bool conn_t::conn_open(size_t id, bool create)
{
    struct sockaddr_un saddr = {AF_UNIX, SERVER_PATH};
    if (_is_open)
    {
        if (create)
        {
            if ((_sd2 = accept(_sd1, NULL, NULL)) == -1)
            {
                syslog(LOG_ERR, "sock: accept failed.");
                close(_sd1);
                unlink(SERVER_PATH);
                return (_is_open = false);
            }
            syslog(LOG_ERR, "sock: accepted.");
        }
        else
        {
            if ((connect(_sd2, (struct sockaddr *)&saddr, SUN_LEN(&saddr))) == -1)
            {
                syslog(LOG_ERR, "sock: connect failed.");
                close(_sd2);
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
        if ((_sd1 = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        {
            syslog(LOG_ERR, "sock: socket failed.");
            return (_is_open = false);
        }
        if ((bind(_sd1, (struct sockaddr *)&saddr, SUN_LEN(&saddr))) == -1)
        {
            syslog(LOG_ERR, "sock: bind failed.");
            close(_sd1);
            return (_is_open = false);
        }
        if ((listen(_sd1, 1)) == -1)
        {
            syslog(LOG_ERR, "sock: listen failed.");
            close(_sd1);
            unlink(SERVER_PATH);
            return (_is_open = false);
        }
        if ((_sd2 = accept(_sd1, NULL, NULL)) == -1)
        {
            syslog(LOG_ERR, "sock: accept failed.");
            close(_sd1);
            unlink(SERVER_PATH);
            return (_is_open = false);
        }
        syslog(LOG_ERR, "sock: accepted.");
    }
    else
    {
        syslog(LOG_NOTICE, "sock: getting connection with id %lu.", id);
        _sd1 = -1;
        if ((_sd2 = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        {
            syslog(LOG_ERR, "sock: socket failed.");
            return (_is_open = false);
        }
        if ((connect(_sd2, (struct sockaddr *)&saddr, SUN_LEN(&saddr))) == -1)
        {
            syslog(LOG_ERR, "sock: connect failed.");
            close(_sd2);
            return (_is_open = false);
        }
        syslog(LOG_ERR, "sock: connected.");
    }

    _is_open = (_sd2 != -1);
    if (_is_open)
        syslog(LOG_ERR, "sock: opened connection with id %lu.", id);
    else
        syslog(LOG_ERR, "sock: couldn't open connection with id %lu.", id);
    
    return _is_open;
}

bool conn_t::conn_close() 
{
    if (_is_open && !close(_sd2) && (_sd1 != -1 && !close(_sd1)) && (!_need_to_rm || (_need_to_rm && !unlink(SERVER_PATH)))) 
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
    if (recv(_sd2, buf, count, 0) == -1)
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
    if (send(_sd2, buf, count, MSG_NOSIGNAL) == -1) 
    {
        syslog(LOG_ERR, "sock: send failed.");
        return false;
    }
    syslog(LOG_NOTICE, "sock: wrote data.");
    return true;
}
