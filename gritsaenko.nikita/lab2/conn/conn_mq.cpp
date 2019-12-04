#include "conn.h"
#include "message.h"
#include <stdlib.h>
#include <syslog.h>
#include <sys/types.h>
#include <fcntl.h>
#include <mqueue.h>
#include <iostream>
#include <cerrno>
#include <string.h>

const char* Q_NAME = "/mqname";

int conn_t::create_connection(bool isHost, bool create) {
	is_owner = isHost;
	id = -1;

    int mq_flags = O_RDWR;

	if (is_owner) {
		mq_flags |= O_CREAT;
		mq_attr attr;  
		attr.mq_flags = 0;  
		attr.mq_maxmsg = 1;  
		attr.mq_curmsgs = 0;  
		attr.mq_msgsize = sizeof(message_t);

		if ((id = mq_open(Q_NAME, mq_flags, 0666, &attr)) == -1) {
			syslog(LOG_ERR, "Error: host failed to access to message queue");
			syslog(LOG_ERR, "Error: %s", strerror(errno));

			return -1;
		}
	}
	else {
		if ((id = mq_open(Q_NAME, mq_flags)) == -1) {
			syslog(LOG_ERR, "Error: client failed to access to message queue");
			syslog(LOG_ERR, "Error: %s", strerror(errno));

			return -1;
		}
	}

	return 0;
}

int conn_t::write(message_t *buf, size_t size) {
	if (mq_send(id, (char*) buf, size, 0) == -1) {
		syslog(LOG_ERR, "Error: in conn_t::write mq_send failed");
		syslog(LOG_ERR, "Error: %s", strerror(errno));
		return -1;
	}
	return 0;
}

int conn_t::read(message_t *buf, size_t size) {
	if (mq_receive(id, (char*) buf, size, 0) == -1) {
		syslog(LOG_ERR, "Error: in conn_t::read mq_receive failed");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

		return -1;
	}
	return 0;
}

int conn_t::destroy_connection() {
	if (!is_owner) {
		return 0;
	}

	if (mq_close(id) != 0) {
		syslog(LOG_ERR, "Error: conn_t::destroy_connection : mq_close failed");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

		return -1;
	}
	
	syslog(LOG_INFO, "Info: mq-connection destroyed");
	return 0;
}