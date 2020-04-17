#ifndef DAEMON_H
#define DAEMON_H

#include "file_deleter.h"
#include "config_reader.h"

#include <iostream>
#include <fstream>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <list>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <unistd.h>
#include <syslog.h>


class Daemon {
public:
	void init(int, char**);
	void exec();
	void update();
	static Daemon &getInstance(){
		static Daemon d;
		return d;
	}
	
private:
	void make_fork();
	void update_pid_log();
	
	ConfigReader config;
	FileDeleter fd;
	double wait_sec = 120.0;
};

#endif
