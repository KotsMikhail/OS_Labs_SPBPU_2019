#include "conn.h"

#include <sys/stat.h>
#include <string>
#include <fcntl.h>

char const * const FIFO_PATH = "fifo";

Conn::Conn(pid_t pid, bool create) {
	_pid = pid;
	_create = create;

	if (_create) {
		// create a FIFO special file with FIFO's permissions
		// S_IRUSR, S_IRGRP, S_IROTH - the owner of the file, the group owner of the file, other users can read file
		// S_IWUSR, S_IWGRP, S_IWOTH - the owner of the file, the group owner of the file, other users can write to file
		int err_code = mkfifo(FIFO_PATH, \
			S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if (err_code == -1) {
			ReportError("Error in conn_fifo.cpp ctor: error in mkfifo");
		}
	}

	// O_RDWR - open file for reading and writing
	_fileDescriptor[0] = open(FIFO_PATH, O_RDWR);
	if (_fileDescriptor[0] == -1) {
		ReportError("Error in conn_fifo.cpp ctor: error in open");
	}

	if (_create) {
		ReportMessage("The host opened the connection");
	} else {
		ReportMessage("The client opened the connection");
	}
}

void Conn::Write(int num) {
	int err_code = write(_fileDescriptor[0], &num, sizeof(int));

	if (err_code == -1) {
		ReportError("Error in conn_fifo.cpp Write: error in write");
	}
}

void Conn::Read(int &num) {
	int n;

	int err_code = read(_fileDescriptor[0], &n, sizeof(int));

	if (err_code == -1) {
		ReportError("Error in conn_fifo.cpp Read: error in read");
	}

	num = n;
}

Conn::~Conn() {
	if (_create) {
		if (close(_fileDescriptor[0]) == -1) {
			ReportError("Error in conn_fifo.cpp dtor: error in close");
		}

		if (unlink(FIFO_PATH) == -1) {
			ReportError("Error in conn_fifo.cpp dtor: error in unlink");
		}

		ReportMessage("The host closed the connection");
	}
}