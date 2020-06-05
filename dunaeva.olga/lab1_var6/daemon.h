#ifndef DAEMON_H
#define DAEMON_H

#include <string>

class Daemon
{
public:
    static void Create(char* conf);
    static void DeleteSubDir();
    static void Wait();
private:
    static void DeleteDir(std::string dirPath);
    static void HandleSignal(int signal);
    static void SetPidToFile();
    static void CheckPidFile();
    static void Init(char* conf);

    static const std::string PID_PATH;
};

#endif //DAEMON_H



