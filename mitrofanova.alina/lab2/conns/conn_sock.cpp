#include "conn.h"

#include <signal.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>

char const * const SOCK_PATH = "sock";

Conn::Conn(pid_t pid, bool create) {
	_pid = pid;
	_create = create;

	// represent a UNIX domain socket address
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	// AF_UNIX - local communication
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);
	if (_create) {
		// create an endpoint for communication
		// SOCK_STREAM - sequenced, reliable, two-way, connection-based byte streams
		_fileDescriptor[0] = socket(AF_UNIX, SOCK_STREAM, 0);

		if (_fileDescriptor[0] == -1) {
			ReportError("Error in conn_sock.cpp ctor: error in socket");
		}

		// assign the address specified by addr to the socket
		if (bind(_fileDescriptor[0], (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
			ReportError("Error in conn_sock.cpp ctor: error in bind");
		}

		// mark the socket as a passive socket
		if (listen(_fileDescriptor[0], 1) == -1) {
			ReportError("Error in conn_sock.cpp ctor: error in listen");
		}

		// extract the first connection request on the queue of pending connections for the listening socket
		// create a new connected socket
		_fileDescriptor[1] = accept(_fileDescriptor[0], nullptr, nullptr);

		if (_fileDescriptor[1] == -1) {
			ReportError("Error in conn_sock.cpp ctor: error in accept");
		}

		ReportMessage("The host opened the connection");
	} else {
		_fileDescriptor[1] = socket(AF_UNIX, SOCK_STREAM, 0);

		if (_fileDescriptor[1] == -1) {
			ReportError("Error in conn_sock.cpp ctor: error in socket");
		}

		// connect the socket to the address
		if (connect(_fileDescriptor[1], (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
			ReportError("Error in conn_sock.cpp ctor: error in connect");
		}

		ReportMessage("The client opened the connection");
	}
}

void Conn::Write(int num) {
	int err_code = send(_fileDescriptor[1], &num, sizeof(int), MSG_NOSIGNAL);

	if (err_code == -1) {
		ReportError("Error in conn_sock.cpp Write: error in send");
	}
}

void Conn::Read(int &num) {
	int n;

	int err_code = recv(_fileDescriptor[1], &n, sizeof(int), 0);

	if (err_code == -1) {
		ReportError("Error in conn_sock.cpp Read: error in recv");
	}

	num = n;
}

Conn::~Conn() {
	if (_create) {
		if (close(_fileDescriptor[1]) == -1) {
			ReportError("Error in conn_sock.cpp dtor: error in close 1");
		}

		if (close(_fileDescriptor[0]) == -1) {
			ReportError("Error in conn_sock.cpp dtor: error in close 0");
		}

		if (unlink(SOCK_PATH) == -1) {
			ReportError("Error in conn_sock.cpp dtor: error in unlink");
		}

		ReportMessage("The host closed the connection");
	}
}