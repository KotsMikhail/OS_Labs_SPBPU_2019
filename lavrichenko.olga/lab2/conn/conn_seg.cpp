#include "conn.h"
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <cstring>
#include <iostream>

static void *shared_memory;
const key_t key = 2;

void Connection::open(const std::string &id, bool create) {
	m_is_owner = create;
	m_id = id;

	int shm_flags = 0666;
	if (m_is_owner) {
		shm_flags |= IPC_CREAT;
	}
	
	if ((m_desc[0] = shmget(key, sizeof(int), shm_flags)) < 0) {
		syslog(LOG_ERR, "Error: failed to create shared memory segment");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

		return;
	}

	void* m = shmat(m_desc[0], NULL, 0);
	if (m == (void*) -1) {
		syslog(LOG_ERR, "Error: Connection::create_connection : shmat failed");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

		return;
	}
	
	shared_memory = m;
}

void Connection::write(int message) {
        int *buf = &message;
	if (!shared_memory) {
		syslog(LOG_ERR, "Error: no memory to write to (seg)");
		return;
	}

	memcpy(shared_memory, buf, sizeof(int));
}

int Connection::read(void) {
        int buf[1];
	if (!shared_memory) {
		syslog(LOG_ERR, "Error: no memory to read from (seg)");
		return -1;
	}

	memcpy(buf, shared_memory, sizeof(int));
	return *buf;
}

void Connection::close() {
	
	if (shared_memory && shmdt(shared_memory) != 0) {
		syslog(LOG_ERR, "Error: failed segment detach");
		syslog(LOG_ERR, "Error: %s", strerror(errno));

		return;
	}

	if (!m_is_owner) {
		return;
	}

	if (m_desc[0] < 0) {
		syslog(LOG_ERR, "Error: Connection::destroy_connection : no id given");
		return;
	}

	if (shmctl(m_desc[0], IPC_RMID, NULL) != 0) {
                syslog(LOG_ERR, "Error: failed to delete segment");
		return;
        }

	syslog(LOG_INFO, "Info: seg-connection destroyed");

	shared_memory = NULL;
	m_desc[0] = -1;

	return;
}
