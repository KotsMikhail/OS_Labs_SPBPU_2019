#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <semaphore.h>
#include <cstdlib>
#include <signal.h>
#include <syslog.h>

class Semaphore {
public:
	// Create and delete
	static sem_t* Init(int shm);
	static void Destroy(sem_t* sem);

	// Utils
	static void Post(sem_t* sem);
	static void Wait(sem_t* sem);
	static void TimedWait(sem_t* sem);

private:
	static void ReportMessage(const char* mess) {
		syslog(LOG_INFO, "%s", mess);
	}

	static void ReportError(const char* mess) {
		syslog(LOG_ERR, "%s", mess);
		exit(EXIT_FAILURE);
	}
};


#endif // SEMAPHORE_H
