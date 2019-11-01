#define _XOPEN_SOURCE 500
#include <syslog.h>
#include <sys/resource.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <ftw.h>
#include <fstream>

static const int MAX_PATH = 100;
static const char* PID_PATH = "/var/run/daemon.pid";

char abs_config_path[MAX_PATH];
char abs_folder1[MAX_PATH];
char abs_folder2[MAX_PATH];
char total_log_path[MAX_PATH];
int interval_sec = -1;

void get_config() {
	if (access(abs_config_path, F_OK)) {
		syslog(LOG_ERR, "Can't find config file %s", abs_config_path);
		exit(1);		
	}			
		
	FILE* conf = fopen(abs_config_path, "r");
	if (conf == NULL) {
		syslog(LOG_ERR, "Failed to open config file. Error %d", errno);
		exit(1);
	}
	if (fscanf(conf, "%s", abs_folder1) < 0 || fscanf(conf, "%s", abs_folder2) < 0 || fscanf(conf, "%d", &interval_sec) < 0) {
		syslog(LOG_ERR, "Failed to read config file. Error %d", errno);
		fclose(conf);
		exit(1);
	} 
	fclose(conf);
	
	strcpy(total_log_path, abs_folder2);
	strcat(total_log_path, "/total.log");	
}

bool file_is_log(char* fpath) {
	char *dot = strrchr(fpath, '.');
	return dot && !strcmp(dot, ".log");
}

int copy_and_remove_log(const char* source_path, const struct stat *st, int typeflag, struct FTW *ftwbuf) {
	if (typeflag) {
		return 0;	
	}

	char filename[MAX_PATH];
	strcpy(filename, basename(strdup(source_path))); 
	
	if (!file_is_log(filename)) {
		return 0;
	}
	
	std::ifstream source(source_path);
	if (!source) {
		syslog(LOG_ERR, "Can't open %s", source_path);
		return 0;			
	}

	std::ofstream target(total_log_path, std::fstream::app);

	if (target.tellp() != target.beg) {
		target << std::endl << std::endl;
	}

	target << filename << std::endl << std::endl;
	target << source.rdbuf();

	source.close();
	target.close();
		
	if (remove(source_path)) {
		syslog(LOG_ERR, "Can't remove %s", source_path);
		return 0;		
	}
	
	return 0;
}

void proc() {	
	DIR* dir1 = opendir(abs_folder1);

	if (dir1 == NULL) {
		syslog(LOG_ERR, "Can't open source directory %s", abs_folder1);
		exit(1);		
	} else {
		closedir(dir1);		
	}

	DIR* dir2 = opendir(abs_folder2);
	if (dir2 == NULL) {
		mkdir(abs_folder2, 0755);
	} else {
		closedir(dir2);
	}

	nftw(abs_folder1, copy_and_remove_log, 64, FTW_DEPTH | FTW_PHYS);
}

void sig_handler(int signo)
{
	if (signo == SIGTERM)
	{
		syslog(LOG_INFO, "SIGTERM has been caught! Exiting...");
		FILE* pid_file = fopen(PID_PATH, "w");
		if (pid_file != NULL)
		{
			fclose(pid_file);
		}
		exit(0);
	}

	if (signo == SIGHUP) {	
		syslog(LOG_INFO, "SIGHUP has been caught! Reading config file...");
		get_config();
	} 
}

void handle_signals()
{
	if(signal(SIGTERM, sig_handler) == SIG_ERR)	{
		syslog(LOG_ERR, "Error! Can't catch SIGTERM");
		exit(1);
	}
	if(signal(SIGHUP, sig_handler) == SIG_ERR) {
		syslog(LOG_ERR, "Error! Can't catch SIGHUP");
		exit(1);	
	}
}

int kill_daemon() {
	if (access(PID_PATH, F_OK) != -1) {
		FILE* pid_file = fopen(PID_PATH, "r");
		if (pid_file == NULL) {
			syslog(LOG_ERR, "Can't read pid file. Error %d", errno);
			exit(1);			
		}		
		pid_t pid;
		fscanf(pid_file, "%d", &pid);
		fclose(pid_file);
		
		char proc_path[15];
		sprintf(proc_path, "/proc/%d", pid);
		
		struct stat st;
		if (stat(proc_path, &st) == 0 && S_ISDIR(st.st_mode)) {
			syslog(LOG_INFO, "Found daemon with pid %d", pid);
			syslog(LOG_INFO, "Sending SIGTERM...");
			kill(pid, SIGTERM);		
			return 0;
		} else {
			syslog(LOG_INFO, "No daemons were found");
			return 1;
		}
	} else {
		syslog(LOG_INFO, "No daemons were found");
		return 1;
	}
}

void daemonize()
{
	syslog(LOG_INFO, "Starting daemonization.");

	//First fork
	pid_t pid = fork();
	if (pid < 0) {
		exit(1);
	} else if (pid > 0) {
		exit(0);
	}

	syslog(LOG_INFO, "First fork");

	//Create a new session
	pid_t sid = setsid();
	if (sid < 0) {
		exit(1);
	}
	syslog(LOG_INFO, "New session");

	//Second fork
	pid = fork();
	if (pid < 0) {
		exit(1);
	} else if(pid > 0) {
		exit(0);
	}
	syslog(LOG_INFO, "Second fork");

	pid = getpid();

	//Change working directory to root directory
	chdir("/");

	//Grant all permisions for all files and directories created by the daemon
	umask(0);

	//Redirect std IO
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	

	kill_daemon(); 	

	//Rewrite pid file
	FILE* pid_fp = fopen(PID_PATH, "w");
	if (pid_fp == nullptr) {
		syslog(LOG_ERR, "Failed to open pid file");
		exit(1);
	} 
	fprintf(pid_fp, "%d", pid);	
	fclose(pid_fp);
}

int main(int argc, char* argv[])
{

	if (argc != 2) {
		printf("Incorrect arguments");
		return 0;	
	}
	
	openlog("daemon_log", LOG_NOWAIT | LOG_PID, LOG_USER);

	if (strcmp(argv[1], "stop") == 0) {
		syslog(LOG_INFO, "Stopping...");
		kill_daemon();
		return 0;	
	}

	if (strcmp(argv[1], "start") == 0) {
		syslog(LOG_INFO, "Starting...");
		
		char* config_path = argv[2];
		realpath(config_path, abs_config_path);
		get_config();
		
		daemonize();
		handle_signals();

		while(1) {
			syslog(LOG_INFO, "Processing...");	
			proc();
			syslog(LOG_INFO, "Done");

			sleep(interval_sec);
		}
	}

	return 0;
}
