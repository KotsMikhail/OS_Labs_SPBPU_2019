#ifndef __DAEMON__
#define __DAEMON__

#include "Utils.h"

class Daemon
{
public:
    static const time_t SENILITY = 180;
    static const std::string PID_PATH;

    static Daemon &GetDaemon();

    static void Daemonize(void (*signal_handler)(int));

    void SetConfigPath(char *const config_path);

    void ReadConfig();

    void Run();

private:
    bool _paused = true;
    std::string _config_path = "";
    std::string _from_folder_path = "";
    std::string _to_folder_path = "";
    std::string _new_path = "";
    std::string _old_path = "";
    int _time_interval = 0;

    static void MyFork(int fork_number);

    static void ProcessPid();

    void PrepareDirectories();

    void AgeDepCpy();

    Daemon() = default;
    ~Daemon() = default;
    Daemon(const Daemon &) = delete;
    Daemon &operator=(const Daemon &) = delete;
};

#endif