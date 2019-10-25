#include <syslog.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

using namespace std;

string pid_path = "/var/run/daemon_lab1.pid";
string config_path;
string from_folder_path;
string to_folder_path;
unsigned time_interval;

class Utils
{
public:
	static void ReadConfig()
	{
		ifstream config(config_path);
		if (!config.is_open())
		{
			syslog(LOG_ERR, "Failed to open config file %s", config_path.c_str());
			exit(EXIT_FAILURE);
		}

		from_folder_path.clear();
		to_folder_path.clear();
		time_interval = 0;
		config >> from_folder_path >> to_folder_path >> time_interval;

		if (!(from_folder_path.length() && to_folder_path.length() && time_interval))
		{
			syslog(LOG_ERR, "Invalid config format");
			exit(EXIT_FAILURE);
		}
		config.close();
	}

	static void MyFork(int fork_number)
	{
		pid_t pid = fork();
		if (pid < 0)
		{
			syslog(LOG_ERR, "Fork %d failed", fork_number);
			exit(EXIT_FAILURE);
		}
		if (pid > 0)
			exit(EXIT_SUCCESS);
	}

	static void FileCopy(const string from, const string to)
	{
		ifstream from_stream(from);
		if (!from_stream.is_open())
		{
			syslog(LOG_ERR, "Failed to open form file %s (read)", from.c_str());
			return;
		}
		ofstream to_stream(to);
		if (!to_stream.is_open())
		{
			syslog(LOG_ERR, "Failed to open to file %s (write)", to.c_str());
			from_stream.close();
			return;
		}
		to_stream << from_stream.rdbuf();
		from_stream.close();
		to_stream.close();
	}

	static bool FolderExists(const string path)
	{
		struct stat folder_stat;
		return stat(path.c_str(), &folder_stat) == 0 && S_ISDIR(folder_stat.st_mode);
	}

	static void CreateFolder(const string path)
	{
		if (FolderExists(path))
		{
			//syslog(LOG_NOTICE, "Folder already exists %s", path.c_str());
		}
		else if (mkdir(path.c_str(), ACCESSPERMS) != 0)
		{
			syslog(LOG_ERR, "Failed to create folder %s", path.c_str());
			exit(EXIT_FAILURE);
		}
	}

	static DIR *OpenFolder(const string path)
	{
		DIR *folder = opendir(path.c_str());
		if (folder == nullptr)
		{
			syslog(LOG_ERR, "Failed to open directory %s", path.c_str());
			exit(EXIT_FAILURE);
		}
		return folder;
	}

	static void ClearFolder(const string path)
	{
		DIR *folder = OpenFolder(path);
		struct dirent *file;
		string file_path;
		while ((file = readdir(folder)) != nullptr)
		{
			file_path = path + "/" + file->d_name;
			if (remove(file_path.c_str()) != 0)
			{
				//syslog(LOG_ERR, "Failed to remove file %s", file_path.c_str());
			}
		}
		closedir(folder);
	}
};

class Daemon
{
public:
	static void Daemonize()
	{
		Utils::MyFork(1);
		if (setsid() < 0)
		{
			syslog(LOG_ERR, "Failed to create a session");
			exit(EXIT_FAILURE);
		}
		Utils::MyFork(2);

		signal(SIGHUP, SignalHandler);
		signal(SIGTERM, SignalHandler);

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		umask(0);

		if ((chdir("/")) < 0)
		{
			syslog(LOG_ERR, "Failed to change directory");
			exit(EXIT_FAILURE);
		}

		ProcessPid();
		syslog(LOG_NOTICE, "Daemon created");
	}

	static void SignalHandler(int signal)
	{
		switch (signal)
		{
		case SIGHUP:
			syslog(LOG_NOTICE, "Re-reading config");
			_paused = true;
			Utils::ReadConfig();
			PrepareDirectories();
			_paused = false;
			break;
		case SIGTERM:
			syslog(LOG_NOTICE, "Terminating");
			unlink(pid_path.c_str());
			exit(EXIT_SUCCESS);
			break;
		default:
			syslog(LOG_NOTICE, "Unknown signal %d", signal);
			break;
		}
	}

	static void Run()
	{
		Utils::ReadConfig();
		PrepareDirectories();
		_paused = false;
		while (true)
		{
			if (!_paused)
				AgeDepCpy();
			sleep(time_interval);
		}
	}

private:
	static const time_t SENILITY = 180;
	static bool _paused;
	static string _new_path;
	static string _old_path;

	static void ProcessPid()
	{
		pid_t prev_pid = 0;
		ifstream input_pid_file(pid_path);
		if (!input_pid_file.is_open())
		{
			syslog(LOG_ERR, "Failed to open pid file %s (read)", pid_path.c_str());
			exit(EXIT_FAILURE);
		}
		input_pid_file >> prev_pid;
		if (prev_pid > 0)
			kill(prev_pid, SIGTERM);
		input_pid_file.close();

		ofstream output_pid_file(pid_path);
		if (!output_pid_file.is_open())
		{
			syslog(LOG_ERR, "Failed to open pid file %s (write)", pid_path.c_str());
			exit(EXIT_FAILURE);
		}
		output_pid_file << getpid();
		output_pid_file.close();
	}

	static void AgeDepCpy()
	{
		DIR *folder_from = Utils::OpenFolder(from_folder_path);
		struct dirent *file;
		struct stat file_stat;
		string file_name;
		string file_path;
		time_t current_time;

		time(&current_time);
		Utils::ClearFolder(_old_path);
		Utils::ClearFolder(_new_path);

		while ((file = readdir(folder_from)) != nullptr)
		{
			file_name = file->d_name;
			file_path = from_folder_path + "/" + file_name;

			if (stat(file_path.c_str(), &file_stat) != 0)
			{
				syslog(LOG_ERR, "Failed to get file stat %s", file_path.c_str());
				continue;
			}
			if (S_ISDIR(file_stat.st_mode))
				continue;

			if ((current_time - file_stat.st_atime) < SENILITY)
				Utils::FileCopy(file_path, _new_path + "/" + file_name);
			else
				Utils::FileCopy(file_path, _old_path + "/" + file_name);
		}
		closedir(folder_from);
	}

	static void PrepareDirectories()
	{
		_new_path = to_folder_path + "/" + "NEW";
		_old_path = to_folder_path + "/" + "OLD";
		Utils::CreateFolder(_new_path);
		Utils::CreateFolder(_old_path);
	}
};

bool Daemon::_paused = true;
string Daemon::_new_path = "";
string Daemon::_old_path = "";

int main(int argc, char *argv[])
{
	openlog("daemon_lab1", LOG_PID, LOG_DAEMON);
	if (argc < 2)
	{
		syslog(LOG_ERR, "Not enough arguments");
		exit(EXIT_SUCCESS);
	}
	config_path = realpath(argv[1], nullptr);
	Daemon::Daemonize();
	Daemon::Run();
}