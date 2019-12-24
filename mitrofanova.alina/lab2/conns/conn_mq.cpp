#include "conn.h"

#include <signal.h>
#include <string>
#include <mqueue.h>
#include <syslog.h>

char const * const MQ_PATH = "/mq";

Conn::Conn(pid_t pid, bool create) {
	_pid = pid;
	_create = create;

	if (_create) {
		// structure which is used in getting and setting the attributes of message queue
		// {flags, max # of messages on queue, max message size, # of messages currently in queue}
		struct mq_attr attr = {0, 1, sizeof(int), 0};
		// create a new POSIX message queue or open an existing queue
		// O_RDWR - open file for reading and writing
		// O_CREAT - if MQ_PATH does not exist, create it as a regular file
		// S_IRUSR, S_IRGRP, S_IROTH - the owner of the file, the group owner of the file, other users can read file
		// S_IWUSR, S_IWGRP, S_IWOTH - the owner of the file, the group owner of the file, other users can write to file
		_fileDescriptor[0] = mq_open(MQ_PATH, \
			O_RDWR | O_CREAT, \
			S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, \
			&attr);
	} else {
		_fileDescriptor[0] = mq_open(MQ_PATH, O_RDWR);
	}

	if (_fileDescriptor[0] == -1) {
		ReportError("Error in conn_mq.cpp ctor: error in mq_open");
	}

	if (_create) {
		ReportMessage("The host opened the connection");
	} else {
		ReportMessage("The client opened the connection");
	}
}

void Conn::Write(int num) {
	int err_code = mq_send(_fileDescriptor[0], (const char*)(&num), sizeof(int), 0);

	if (err_code == -1) {
		ReportError("Error in conn_mq.cpp Write: error in mq_send");
	}
}

void Conn::Read(int &num) {
	int n;

	int err_code = mq_receive(_fileDescriptor[0], (char*)(&n), sizeof(int), nullptr);

	if (err_code == -1) {
		ReportError("Error in conn_mq.cpp Read: error in mq_recieve");
	}

	num = n;
}

Conn::~Conn() {
	if (_create) {
		if (mq_close(_fileDescriptor[0]) == -1) {
			ReportError("Error in conn_mq.cpp dtor: error in mq_close");
		}

		if (mq_unlink(MQ_PATH) == -1) {
			ReportError("Error in conn_mq.cpp dtor: error in mq_unlink");
		}

		ReportMessage("The host closed the connection");
	}
}