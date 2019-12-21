#ifndef CONN_H_INCLUDED
#define CONN_H_INCLUDED

#include <unistd.h>
#include <syslog.h>
#include <cstdlib>
#include <signal.h>

class Conn {
public:
	Conn(pid_t pid, bool create);
	void Write(int num);
	void Read(int &num);
	~Conn();
private:
	pid_t _pid;
	bool _create;
	int _fileDescriptor[2];

	void ReportError(const char *mess) {
		syslog(LOG_ERR, "%s", mess);
		kill(_pid, SIGTERM);
		exit(EXIT_FAILURE);
	}

	void ReportMessage(const char* mess) {
		syslog(LOG_INFO, "%s", mess);
	}
};

#endif //CONN_H_INCLUDED
