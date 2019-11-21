#include <fcntl.h>
#include <syslog.h>
#include <sys/stat.h>
#include <unistd.h>

#include "connect.h"
#include "message.h"

#define FIFO_PATH "/tmp/lab2_fifo"

bool _need_to_rm = true, _is_open = false;
int fd = -1;

bool conn_t::conn_open(size_t id, bool create)
{
    if (_is_open)
        return true;
        
    _need_to_rm = create;
    
    if (create)
    {
        syslog(LOG_NOTICE, "fifo: creating connection with id %lu.", id); 
        if (mkfifo(FIFO_PATH, 0666) == -1)
        {
            syslog(LOG_ERR, "fifo: mkfifo failed.");
            return (_is_open = false);
        }
    }
    else
        syslog(LOG_NOTICE, "fifo: getting connection with id %lu.", id);

    fd = open(FIFO_PATH, O_RDWR);

    _is_open = (fd != -1);
    if (_is_open)
        syslog(LOG_ERR, "fifo: opened connection with id %lu.", id);
    else
    {
        syslog(LOG_ERR, "fifo: couldn't open connection with id %lu.", id);
        if (_need_to_rm)
            unlink(FIFO_PATH);
    }
    
    return _is_open;
}

bool conn_t::conn_close() 
{
    if (_is_open && !close(fd) && (!_need_to_rm || (_need_to_rm && !unlink(FIFO_PATH)))) 
    {
        syslog(LOG_NOTICE, "fifo: closed.");
        _is_open = false;
    }
    return !_is_open;
}

bool conn_t::conn_recv(void *buf, size_t count)
{
    if (!_is_open)
    {
        syslog(LOG_ERR, "fifo: couldn't read data.");
        return false;
    }
    if (read(fd, buf, count) == -1)
    {
        syslog(LOG_ERR, "fifo: read failed.");
        return false;
    }
    syslog(LOG_NOTICE, "fifo: read data.");
    return true;
}

bool conn_t::conn_send(void *buf, size_t count)
{
    if (!_is_open)
    {
        syslog(LOG_ERR, "fifo: couldn't write data.");
        return false;
    }
    if (write(fd, buf, count) == -1) 
    {
        syslog(LOG_ERR, "fifo: write failed.");
        return false;
    }
    syslog(LOG_NOTICE, "fifo: wrote data.");
    return true;
}
