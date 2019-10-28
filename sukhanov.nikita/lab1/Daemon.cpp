#include "Daemon.h"

using namespace std;

const string Daemon::PID_PATH("/var/run/daemon_lab1.pid");

Daemon &Daemon::GetDaemon()
{
    static Daemon daemon_instance;
    return daemon_instance;
}

void Daemon::Daemonize(void (*signal_handler)(int))
{
    MyFork(1);
    if (setsid() < 0)
    {
        syslog(LOG_ERR, "Failed to create a session");
        exit(EXIT_FAILURE);
    }
    MyFork(2);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

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

void Daemon::SetConfigPath(char *const config_path)
{
    char *real_path = realpath(config_path, nullptr);
    if (real_path == nullptr)
    {
        syslog(LOG_ERR, "Failed to get real path %s", config_path);
        _config_path = "";
    }
    else
        _config_path = real_path;
}

void Daemon::ReadConfig()
{
    _paused = true;
    ifstream config(_config_path);
    if (!config.is_open())
    {
        syslog(LOG_ERR, "Failed to open config file %s", _config_path.c_str());
        exit(EXIT_FAILURE);
    }

    _from_folder_path.clear();
    _to_folder_path.clear();
    _time_interval = 0;
    config >> _from_folder_path >> _to_folder_path >> _time_interval;

    if (!(_from_folder_path.length() && _to_folder_path.length() && _time_interval > 0))
    {
        syslog(LOG_ERR, "Invalid config format");
        exit(EXIT_FAILURE);
    }
    config.close();
    PrepareDirectories();
    _paused = false;
}

void Daemon::Run()
{
    ReadConfig();
    _paused = false;
    while (true)
    {
        if (!_paused)
            AgeDepCpy();
        sleep(_time_interval);
    }
}

void Daemon::MyFork(int fork_number)
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

void Daemon::ProcessPid()
{
    pid_t prev_pid = 0;
    ifstream input_pid_file(PID_PATH);
    if (!input_pid_file.is_open())
    {
        syslog(LOG_ERR, "Failed to open pid file %s (read)", PID_PATH.c_str());
        exit(EXIT_FAILURE);
    }
    input_pid_file >> prev_pid;
    if (prev_pid > 0)
        kill(prev_pid, SIGTERM);
    input_pid_file.close();

    ofstream output_pid_file(PID_PATH);
    if (!output_pid_file.is_open())
    {
        syslog(LOG_ERR, "Failed to open pid file %s (write)", PID_PATH.c_str());
        exit(EXIT_FAILURE);
    }
    output_pid_file << getpid();
    output_pid_file.close();
}

void Daemon::PrepareDirectories()
{
    _new_path = _to_folder_path + "/" + "NEW";
    _old_path = _to_folder_path + "/" + "OLD";
    Utils::CreateFolder(_new_path);
    Utils::CreateFolder(_old_path);
}

void Daemon::AgeDepCpy()
{
    DIR *folder_from = Utils::OpenFolder(_from_folder_path);
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
        file_path = _from_folder_path + "/" + file_name;

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
