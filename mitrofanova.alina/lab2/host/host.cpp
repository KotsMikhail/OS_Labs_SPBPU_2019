#include <syslog.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

#include "ShM.h"
#include "Semaphore.h"
#include "wolf.h"
#include "goat.h"

int main(int argc, char** argv) {
	openlog("Game 'The wolf and the seven goats'", LOG_CONS | LOG_PID, LOG_USER);
	syslog(LOG_INFO, "The host is running");

	const char* hostShmName = "host_shm";
	int hostShm = ShM::Open(hostShmName);
	sem_t* hostSem = Semaphore::Init(hostShm);
	
	const char* clientShmName = "client_shm";
	int clientShm = ShM::Open(clientShmName);
	sem_t* clientSem = Semaphore::Init(clientShm);

	// create process
	pid_t pid = fork();

	if (pid == -1) {
		syslog(LOG_ERR, "MAIN ERROR fork");
		exit(EXIT_FAILURE);
	}

	if (pid != 0) {
		syslog(LOG_INFO, "The host launched the client");

		Wolf &wolf = Wolf::GetInstance();

		wolf.Working(hostSem, clientSem);

		Semaphore::Destroy(hostSem);
		Semaphore::Destroy(clientSem);
		
		ShM::Unlink(hostShmName);
		ShM::Unlink(clientShmName);

		kill(pid, SIGTERM);
		syslog(LOG_INFO, "Client completed");
	} else {
		sleep(1);

		Goat &goat = Goat::GetInstance();
		
		goat.Working(hostSem, clientSem);
	}
}