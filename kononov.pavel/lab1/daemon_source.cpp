#include <iostream>
#include <fstream>
#include <csignal>
#include <cstdlib>
#include <sys/stat.h>
#include <ctime>
#include <fcntl.h>
#include <list>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <unistd.h>
#include <syslog.h>
#include <dirent.h>

#define PID_LOGS "/var/run/daemon_lab_5.pid"


struct ConfigReader {
	void setPath(std::string path) { config_path = path; }
	void read();
	std::string getDirPath() { return dir_path; }

private:
	std::string config_path;
	std::string dir_path;
} config_reader;


class FileDeleter {
public:
	void setPath(std::string path) {
		this->path = path;
	}
	
	void exec();
	
private:
	std::string path;
	const double treshold_seconds = 60.0;
};

void FileDeleter::exec()
{
	std::time_t now = std::time(0);   // get time now
	struct stat sb;
	struct dirent *entry;
	DIR *dp;

	dp = opendir(path.c_str());
	if (dp == NULL) 
	{
		syslog(LOG_ERR, "Could not open directory");
		exit(EXIT_FAILURE);
	}

	while((entry = readdir(dp)))
	{
		std::string p = path;
		p += "/";
		p += entry->d_name;
		if (stat(p.c_str(), &sb) != 0) {
			syslog(LOG_ERR, "Could not open file");
			exit(EXIT_FAILURE);
		} 
		double seconds = difftime(now, sb.st_ctime);
		if (seconds > treshold_seconds)
		{
			remove(p.c_str());
		}
	}

	closedir(dp);
}

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
	const double wait_sec = 120.0;
};

void Daemon::make_fork()
{
	pid_t pid = fork();
	switch(pid)
	{
		case 0:
			break;
		case -1:
			exit(EXIT_FAILURE);
		default:
			syslog(LOG_NOTICE, "Forked. Child's pid is %d.", pid);
			exit(EXIT_SUCCESS);
	}
}

void signal_handler(int sgnl)
{
	switch(sgnl)
	{
	case SIGHUP:
		Daemon::getInstance().update();
		syslog(LOG_NOTICE, "SIGHUP catched");
		break;
	case SIGTERM:
		syslog(LOG_NOTICE, "SIGTERM catched");
		unlink(PID_LOGS);
		exit(EXIT_SUCCESS);
		break;
	}
}

void Daemon::init(int argc, char** argv)
{
	make_fork();
	
	if (argc < 2) {
		printf("Expected arguments.");
		exit(EXIT_FAILURE);
	}
	
	openlog("daemon_lab_5", LOG_NOWAIT | LOG_PID, LOG_USER);
	umask(0);

	if (setsid() < 0)
	{
		syslog(LOG_ERR, "Could not generate session process");
		exit(EXIT_FAILURE);
	}
	
	make_fork();
	
	config.setPath(realpath(argv[1], nullptr));
	update();

	if ((chdir("/")) < 0)
	{
		syslog(LOG_ERR, "Could not change directory to /");
		exit(EXIT_FAILURE);
    	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

    	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);

	update_pid_log();
}

void Daemon::update_pid_log()
{
	std::ifstream iFile(PID_LOGS);
	if (iFile.is_open() && !iFile.eof())
	{
		syslog(LOG_ERR, "ifstream opened");
		pid_t p;
		iFile >> p;
		if (p > 0)
			kill(p, SIGTERM);
	}
	iFile.close();
	
	std::ofstream oFile(PID_LOGS);
	if (!oFile) {
		syslog(LOG_ERR, "Could not open pid logs");
		exit(EXIT_FAILURE);
	}
	oFile << getpid();
	oFile.close();
}

void Daemon::exec()
{
	while (true)
	{
		fd.exec();
		sleep(wait_sec);
	}
}


void Daemon::update()
{
	config.read();
	fd.setPath(config.getDirPath());
}


void ConfigReader::read()
{
	std::ifstream config = std::ifstream(config_path);
	if (!config.is_open() || config.eof()) {
        syslog(LOG_ERR, "Bad path or empty config");
        exit(EXIT_FAILURE);
    }

	std::string line;
	while (std::getline(config, line))
	{
		DIR *dp;

		dp = opendir(line.c_str());
		if (dp == NULL) 
		{
			syslog(LOG_NOTICE, "Path from config not exist");
			exit(EXIT_FAILURE);
		}
		
		dir_path = line;
		closedir(dp);
	}
	
	config.close();
}


int main(int argc, char** argv)
{
	Daemon instance = Daemon::getInstance();

	instance.init(argc, argv);
	instance.exec();

	return 0;
}
