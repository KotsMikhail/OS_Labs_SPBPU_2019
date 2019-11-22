#include <fcntl.h>
#include <mqueue.h>
#include <syslog.h>

#include "connect.h"
#include "message.h"

#define MQ_NAME             "/lab2_mq"
#define MQ_ATTR_INITIALIZER ((struct mq_attr){0, 1, MESSAGE_SIZE, 0, {0}})

bool _need_to_rm = true, _is_open = false;
mqd_t _mqdes = (mqd_t) -1;

bool conn_t::conn_open(size_t id, bool create)
{
    if (_is_open)
        return true;
        
    _need_to_rm = create;
    
    if (create)
    {
        syslog(LOG_NOTICE, "mq: creating connection with id %lu.", id); 
        struct mq_attr attr = MQ_ATTR_INITIALIZER;
        _mqdes = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0666, &attr);
    }
    else
    {
        syslog(LOG_NOTICE, "mq: getting connection with id %lu.", id);
        _mqdes = mq_open(MQ_NAME, O_RDWR);
    }

    _is_open = (_mqdes != (mqd_t) -1);
    if (_is_open)
        syslog(LOG_ERR, "mq: opened connection with id %lu.", id);
    else
        syslog(LOG_ERR, "mq: couldn't open connection with id %lu.", id);
    
    return _is_open;
}

bool conn_t::conn_close() 
{
    if (_is_open && !mq_close(_mqdes) && (!_need_to_rm || (_need_to_rm && !mq_unlink(MQ_NAME)))) 
    {
        syslog(LOG_NOTICE, "mq: closed.");
        _is_open = false;
    }
    return !_is_open;
}

bool conn_t::conn_recv(void *buf, size_t count)
{
    if (!_is_open || count > MESSAGE_SIZE)
    {
        syslog(LOG_ERR, "mq: couldn't read data.");
        return false;
    }
    if (mq_receive(_mqdes, (char *)buf, count, nullptr) == -1)
    {
        syslog(LOG_ERR, "mq: mq_receive failed.");
        return false;
    }
    syslog(LOG_NOTICE, "mq: read data.");
    return true;
}

bool conn_t::conn_send(void *buf, size_t count)
{
    if (!_is_open || count > MESSAGE_SIZE)
    {
        syslog(LOG_ERR, "mq: couldn't write data.");
        return false;
    }
    if (mq_send(_mqdes, (char *)buf, count, 0) == -1) 
    {
        syslog(LOG_ERR, "mq: mq_send failed.");
        return false;
    }
    syslog(LOG_NOTICE, "mq: wrote data.");
    return true;
}
