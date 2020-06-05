#ifndef DAEMON_H
#define DAEMON_H

#include <string>

class Daemon
{
public:
    Daemon(char *conf);
    void DeleteSubDir();
    void Wait();
private:
    static void HandleSignal(int signal);
    void DeleteDir(std::string dirPath);
    void SetPidToFile();
    void CheckPidFile();
    void Create();

    const std::string PID_PATH = "/var/run/daemon.pid";
};

#endif //DAEMON_H




