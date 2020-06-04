#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>

#include <iostream>

#include "daemon.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "Missing name of config file" << endl;
        exit(EXIT_FAILURE);
    }

    Daemon::Create(argv[1]);
    while (true)
    {
        Daemon::DeleteSubDir();
        Daemon::Wait();
    }
}
