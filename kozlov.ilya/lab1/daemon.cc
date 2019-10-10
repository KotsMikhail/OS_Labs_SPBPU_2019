//
// Created by ilya on 10/10/19.
//

#include "daemon.h"

#include <csignal>
#include <sys/stat.h>
#include <zconf.h>
#include <syslog.h>

// Static members
std::string Daemon::config_file_ = "lab1.conf";
int Daemon::time_interval_ = 30;
std::string Daemon::pid_file_ = "/var/run/lab1.pid";
std::string Daemon::name_ = "LAB1 - DAEMON";

// PUBLIC //

bool Daemon::Init(char* config_file)
{
  config_file_ = config_file;
  openlog(name_.c_str(), LOG_PID | LOG_NDELAY, LOG_LOCAL0);
  SetSignals();
  return LoadConfig();
}

void Daemon::Start()
{
  umask(0);
  setsid();
  chdir("/");
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  StartWork();
}

void Daemon::Terminate()
{
  syslog(LOG_INFO, "Closing...");
  closelog();
  exit(SIGTERM);
}

// PRIVATE //

bool Daemon::LoadConfig()
{
  // TODO: read config file
  syslog(LOG_INFO, "Loading config...");
  time_interval_ = 30;
  pid_file_ = "/var/run/lab1.pid";
  return true;
}

bool Daemon::CheckPidFile()
{
  // TODO: check pid file -> check pid in '/proc'
  return false;
}

bool Daemon::SetPidFile()
{
  // TODO: set pid to pid_file_
  return false;
}

void Daemon::SetSignals()
{
  signal(SIGTERM, SignalHandler);
  signal(SIGHUP, SignalHandler);
}

void Daemon::StartWork()
{
  syslog(LOG_INFO, "Starting...");
  while (true)
  {
    DoWork();
    sleep(time_interval_);
  }
}

void Daemon::DoWork()
{
  // TODO: main work with dirs
  syslog(LOG_INFO, "Working...");
}

void Daemon::SignalHandler(int sig_num)
{
  if (sig_num == SIGTERM)
  {
    Terminate();
  }
  else if (sig_num == SIGHUP)
  {
    LoadConfig();
  }
}
