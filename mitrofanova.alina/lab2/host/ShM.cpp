#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <cstdlib>

#include "ShM.h"

int ShM::Open(const char* str) {
	ReportMessage("ShM: Open");
	// create and open a new, or open an existing, shared memory object
	// O_RDWR - open object for read-write access
	// O_CREAT - create if it does not exist
	// S_IRWXU - read, write, execute, search by owner
	int shm = shm_open(str, O_RDWR | O_CREAT, S_IRWXU);

	if (shm == -1) {
		ReportError("ShM ERROR during shm_open");
	}

	return shm;
}

void ShM::Unlink(const char* str) {
	ReportMessage("ShM: Unlink");
	// remove shared memory object
	if (shm_unlink(str) == -1) {
		ReportError("ShM ERROR during shm_unlink");
	}
}