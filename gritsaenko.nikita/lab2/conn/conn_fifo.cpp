#include "conn.h"
#include "message.h"
#include <stdlib.h>
#include <iostream>
#include <syslog.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

const char* FIFO_FILE = "/tmp/fifo_ipc";

int conn_t::create_connection(bool isHost, bool create) {
	is_owner = isHost;
	id = -1;

    if (is_owner) {
        if (mkfifo(FIFO_FILE, 0666) != 0) {
            syslog(LOG_ERR, "Error: mkfifo failed");
		    syslog(LOG_ERR, "Error: %s", strerror(errno));

            return -1;
        }
    }

    if ((id = open(FIFO_FILE, O_RDWR)) == -1) {
        syslog(LOG_ERR, "Error: fifo: open failed");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

        return -1;
    }
    return 0;
}

int conn_t::write(message_t *buf, size_t size) {
    if (::write(id, buf, size) == -1) {
        syslog(LOG_ERR, "Error: fifo: write failed");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

        return -1;
    }
	return 0;
}

int conn_t::read(message_t *buf, size_t size) {
	if (::read(id, buf, size) == -1) {
        syslog(LOG_ERR, "Error: fifo: read failed");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

        return -1;
    }
	return 0;
}

int conn_t::destroy_connection() {
	if (!is_owner) {
		return 0;
	}
	if (close(id) == -1) {
        syslog(LOG_ERR, "Error: fifo: close fifo failed");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

        return -1;
    }
    if (unlink(FIFO_FILE) == -1) {
        syslog(LOG_ERR, "Error: fifo: delete fifo failed");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

        return -1;
    }
	syslog(LOG_INFO, "Info: fifo-connection destroyed");
	return 0;
}