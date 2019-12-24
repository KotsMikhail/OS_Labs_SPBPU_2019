#include "Semaphore.h"

#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <time.h>
#include <syslog.h>

sem_t* Semaphore::Init(int shm) {
	ReportMessage("Semaphore: Initialization");
	// truncate the file referenced by shm to a sizeof sem_t
	if (ftruncate(shm, sizeof(sem_t)) == -1) {
		ReportError("Semaphore ERROR during ftruncate");
	}

	// create a new mapping in the VAS of the calling process
	// void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
	// prot - the desired memory protection of the mapping
	// PROT_READ, PROT_WRITE - pages may be read, write
	// flags - determine whether updates to the mapping are visible and etc.
	// MAP_SHARED - share this mapping
	sem_t* sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, \
		MAP_SHARED, shm, 0);

	if (sem == MAP_FAILED) {
		ReportError("Semaphore ERROR during mmap");
	}

	// initialize the unnamed semaphore
	// int sem_init(sem_t *sem, int pshared, unsigned int value)
	if (sem_init(sem, 1, 0) == -1) {
		ReportError("Semaphore ERROR during sem_init");
	}

	return sem;
}

void Semaphore::Destroy(sem_t* sem) {
	ReportMessage("Semaphore: Destruction");
	// destroy the unnamed semaphore
	if (sem_destroy(sem) == -1) {
		ReportError("Semaphore ERROR during sem_destroy");
	}
}

void Semaphore::Post(sem_t* sem) {
	ReportMessage("Semaphore: Post");
	// increment (unlock) the semaphore
	if (sem_post(sem) == -1) {
		ReportError("Semaphore ERROR during sem_post");
	}
}

void Semaphore::Wait(sem_t* sem) {
	ReportMessage("Semaphore: Wait");
	// decrement (lock) the semaphore
	if (sem_wait(sem) == -1) {
		ReportError("Semaphore ERROR during sem_wait");
	}
}

void Semaphore::TimedWait(sem_t* sem) {
	ReportMessage("Semaphore: Timed Wait");
	int TIME_WAIT = 5;
	struct timespec abs_timeout;

	if (clock_gettime(CLOCK_REALTIME, &abs_timeout) == -1) {
		ReportError("Semaphore ERROR during clock_gettime");
	}

	abs_timeout.tv_sec += TIME_WAIT;

	// decrement (lock) the semphore with a limit on the amount of time
	// that the call should block if the decrement cannot be immediately performed
	if (sem_timedwait(sem, &abs_timeout) == -1) {
		ReportError("Semaphore ERROR during sem_timedwait");
	}
}