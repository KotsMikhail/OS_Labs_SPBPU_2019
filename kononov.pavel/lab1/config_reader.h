#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <iostream>
#include <syslog.h>
#include <dirent.h>

class ConfigReader {
public:
	void setPath(std::string path) { config_path = path; }
	void read();
	std::string getDirPath() { return dir_path; }
	double getWaitTime() { return wt; }

private:
	std::string config_path;
	std::string dir_path;
	double wt;
};

#endif
