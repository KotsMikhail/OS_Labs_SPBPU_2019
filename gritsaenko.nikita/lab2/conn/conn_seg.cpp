#include "conn.h"
#include "message.h"
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <cstring>
#include <iostream>

static void *shared_memory;
static key_t MESSAGE_KEY = 1;

int conn_t::create_connection(bool isHost, bool create) {
	is_owner = isHost;
	id = -1;

	int shm_flags = 0666 | isHost ? IPC_CREAT : 0;
	
	if ((id = shmget(MESSAGE_KEY, sizeof(message_t), shm_flags)) < 0) {
		syslog(LOG_ERR, "Error: failed to create shared memory segment");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

		return -1;
	}

	void* m = shmat(id, NULL, 0);
	if (m == (void*) -1) {
		syslog(LOG_ERR, "Error: conn_t::create_connection : shmat failed");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

		return -1;
	}
	
	shared_memory = m;
	return 0;
}

int conn_t::write(message_t *buf, size_t size) {
	if (!buf) {
		syslog(LOG_ERR, "Error: in conn_t::write buffer is null");
		return -1;
	}
	if (!shared_memory) {
		syslog(LOG_ERR, "Error: no memory to write to (seg)");
		return -1;
	}

	memcpy(shared_memory, buf, size);

	return 0;
}

int conn_t::read(message_t *buf, size_t size) {
	if (!buf) {
		syslog(LOG_ERR, "Error: in conn_t::read buffer is null");
		return -1;
	}
	if (!shared_memory) {
		syslog(LOG_ERR, "Error: no memory to read from (seg)");
		return -1;
	}

	memcpy(buf, shared_memory, size);
	return 0;
}

int conn_t::destroy_connection() {
	
	if (shared_memory && shmdt(shared_memory) != 0) {
		syslog(LOG_ERR, "Error: failed segment detach");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

		return -1;
	}

	if (!is_owner) {
		return 0;
	}

	if (id < 0) {
		syslog(LOG_ERR, "Error: conn_t::destroy_connection : no id given");
		return -1;
	}

	if (shmctl(id, IPC_RMID, NULL) != 0) {
        syslog(LOG_ERR, "Error: failed to delete segment");
		return -1;
    }

	syslog(LOG_INFO, "Info: seg-connection destroyed");

	shared_memory = NULL;
	id = -1;

	return 0;
}