#ifndef SHM_H
#define SHM_H

#include <cstdlib>
#include <signal.h>
#include <syslog.h>

class ShM {
public:
	// Open and unlink
	static int Open(const char* str);
	static void Unlink(const char* str);

private:
	static void ReportMessage(const char* mess) {
		syslog(LOG_INFO, "%s", mess);
	}

	static void ReportError(const char* mess) {
		syslog(LOG_ERR, "%s", mess);
		exit(EXIT_FAILURE);
	}
};


#endif // SHM_H
