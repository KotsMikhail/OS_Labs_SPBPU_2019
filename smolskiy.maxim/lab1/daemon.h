#ifndef DAEMON_H
#define DAEMON_H

#include <string>

class Daemon
{
public:
    static void ProtectAgainstRestart();
    static void HandleSignal(int sigNum);
    static void DeleteAllSubfolders();

private:
    static const std::string ABS_PID_PATH;
};

#endif //DAEMON_H
