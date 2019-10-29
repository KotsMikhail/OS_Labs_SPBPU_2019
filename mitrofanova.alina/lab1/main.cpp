/*
 * Mitrofanova Alina
 * Lab 1
 * Creating deamons on Linux system
 * Variant 10
*/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>

std::string config_file;
std::string dir1;
std::string dir2;
std::string hist_file = "hist.log";
std::string pid_file = "/var/run/lab1_daemon.pid";
int interval;
int num_space = 0;

void ReadConfigFile() {
	std::ifstream ifs(config_file);

	if (ifs.is_open()) {
		ifs >> dir1 >> dir2 >> interval;
		ifs.close();
	} else {
		syslog(LOG_ERR, "Could not open config file");
		exit(EXIT_FAILURE);
	}

	// hist.log is located in dir2
	hist_file = dir2 + "/hist.log";

	syslog(LOG_INFO, "Read config file. Directory 1: %s. Directory 2: %s. Hist.log: %s",
		dir1.c_str(), dir2.c_str(), hist_file.c_str());
}

void SignalHandler(int signum) {
	switch (signum) {
	case SIGHUP: // re-read config file
		syslog(LOG_INFO, "Catch signal SIGHUP");
		ReadConfigFile();
		break;
	case SIGTERM: // syslog about exit, and exit
		syslog(LOG_INFO, "Catch signal SIGTERM");
		unlink(pid_file.c_str());
		exit(EXIT_SUCCESS);
		break;
	default:
		syslog(LOG_ERR, "Catch unknown signal");
		break;
	}
}

void KillDaemon() {
	std::ifstream ifs(pid_file);
	pid_t pid;

	if (ifs.is_open()) {
		ifs >> pid;
		if (pid == -1) {
			syslog(LOG_ERR, "Error pid and could not kill daemon");
			exit(EXIT_FAILURE);
		} else if (pid > 0) {
			kill(pid, SIGTERM);
			ifs.close();
		}
	} else {
		syslog(LOG_ERR, "Could not open pid file and kill daemon");
		exit(EXIT_FAILURE);
	}
}

void SetPidFile() {
	std::ofstream ofs(pid_file);

	if (ofs.is_open()) {
		ofs << getpid();
		ofs.close();
	} else {
		syslog(LOG_ERR, "Could not set pid file");
		exit(EXIT_FAILURE);
	}
}

char* CurrentTime() {
	char* buffer = (char*)malloc(80);
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	char format[] = "%d %B, %H:%M:%S ";
	strftime(buffer, 80, format, timeinfo);
	return buffer;
}

std::string PrintSpace() {
	std::string buffer;
	for (int i = 0; i < num_space; i++)
		buffer.push_back(' ');
	return buffer;
}

void WriteAll(std::ofstream& ofs, std::string& path) {
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(path.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_DIR) {
				if (!((std::string(ent->d_name)).compare(".") == 0 || (std::string(ent->d_name)).compare("..") == 0)) {
					std::string curr_path = path + "/" + std::string(ent->d_name);
					ofs << PrintSpace() << ent->d_name << " ==>> " << std::endl;
					num_space += 4;
					WriteAll(ofs, curr_path);
					num_space -= 4;
				}
			} else {
				ofs << PrintSpace() << ent->d_name << std::endl;
			}
		}
	} else {
		syslog(LOG_ERR, "Could not open dir %s", path.c_str());
		exit(EXIT_FAILURE);
	}
	closedir(dir);
}

void DoProtocol(std::string& dir1, std::string& dir2) {
	syslog(LOG_INFO, "Do protocol");

	std::ofstream ofs(hist_file, std::ios_base::out | std::ios_base::app); // open file for writing and append
	if (!ofs.is_open()) {
		syslog(LOG_ERR, "Could not open hist.log");
		exit(EXIT_FAILURE);		
	}

	ofs << CurrentTime() << std::endl;

	num_space = 2;
	WriteAll(ofs, dir1);

	ofs.close();
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Wrong number of arguments\n");
		exit(EXIT_FAILURE);
	}
	config_file = argv[1];

	// create process
	pid_t pid = fork();

	if (pid == -1) {
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	// LOG_NOWAIT - don't wait for child processes
	// LOG_PID - include PID with each message
	openlog("lab1_daemon", LOG_NOWAIT | LOG_PID, LOG_USER);

	// reset user mode creation mask
	umask(0);

	// create a new session if the calling process is not a process group leader
	if (setsid() == -1) {
		syslog(LOG_ERR, "Could not create a new session");
		exit(EXIT_FAILURE);
	}

	pid = fork();

	if (pid == -1) {
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	ReadConfigFile();

	// change the current working directory
	if ((chdir("/")) < 0) {
		syslog(LOG_ERR, "Could not change working directory");
		exit(EXIT_FAILURE);
	}

	// close streams
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// signal handler registration
	signal(SIGHUP, SignalHandler);
	signal(SIGTERM, SignalHandler);

	// kill parent
	KillDaemon();
	SetPidFile();

	syslog(LOG_INFO, "Start Deamon");

	while (true)
	{
		DoProtocol(dir1, dir2);
		sleep(interval);
	}

	closelog();
	return EXIT_SUCCESS;
}