//
// Created by peter on 10/25/19.
//

#include "daemon.h"

#include <syslog.h>
#include <csignal>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <cstring>

#include "config_parser.h"

const std::string Daemon::LOG_TAG = "LAB1";

// default work parameter values
unsigned int Daemon::timeInterval = 45;
std::string Daemon::dir1Path = "dir1";
std::string Daemon::dir2Path = "dir2";
std::string Daemon::totalLogPath;

std::string Daemon::pidFileName = "/var/run/lab1.pid";
std::string Daemon::workDir;
std::string Daemon::configFileName;

bool Daemon::start( const string &configFilename )
{
  // open system log
  openlog(LOG_TAG.c_str(), LOG_PID | LOG_NDELAY, LOG_USER);

  // save current working directory
  char buf[FILENAME_MAX];
  getcwd(buf, FILENAME_MAX);
  workDir = buf;
  syslog(LOG_INFO, "Current work dir: %s", buf);

  // save config file absolute path
  Daemon::configFileName = getAbsolutePath(configFilename);

  // set up signal handling
  signal(SIGTERM, signalHandle);
  signal(SIGHUP, signalHandle);

  // parse config file
  if (!parseConfig())
    return false;

  // create session
  if (setsid() == -1)
  {
    syslog(LOG_ERR, "'setsid' returned error: %d", errno);
    terminate();
  }

  // create child process for background work handling
  switch (fork())
  {
  case -1:
    syslog(LOG_ERR, "Work process fork failed");
    terminate();
  case 0:
    syslog(LOG_INFO, "Created child work process");

    // set permissions
    umask(0);

    // change work directory to root
    if (chdir("/") == -1)
    {
      syslog(LOG_ERR, "'chdir' returned error: %d", errno);
      terminate();
    }

    // close file streams
    if (close(STDIN_FILENO) == -1 ||
        close(STDOUT_FILENO) == -1 ||
        close(STDERR_FILENO) == -1)
    {
      syslog(LOG_ERR, "'close' returned error: %d", errno);
      terminate();
    }

    // start work handle loop
    startWorkLoop();
  }

  syslog(LOG_INFO, "Exit parent process");
  clear();

  return true;
} // end of 'Daemon::start' function

std::string Daemon::getAbsolutePath( const string &path )
{
  // check empty path
  if (path.empty())
    return path;

  // check already absolute path
  if (path[0] == '/')
    return path;

  // get absolute path
  string output = workDir + "/" + path;
  char *real = realpath(output.c_str(), nullptr);

  if (real == nullptr)
  {
    syslog(LOG_ERR, "Couldn't get absolute path to '%s'", path.c_str());
    return string();
  }

  output.assign(real);
  free(real);

  return output;
} // end of 'Daemon::getAbsolutePath' function

void Daemon::clear()
{
  // close system log
  syslog(LOG_INFO, "Close log");
  closelog();
} // end of 'Daemon::clear' function

bool Daemon::handlePidFile()
{
  // try to read pid file
  std::ifstream pid_file(Daemon::pidFileName);

  // if already exists
  if (pid_file.is_open())
  {
    // read old pid
    pid_t old_pid;
    pid_file >> old_pid;
    pid_file.close();

    // find old process
    struct stat sb;
    string path_to_old = "/proc/" + std::to_string(old_pid);

    // send terminate signal
    if (stat(path_to_old.c_str(), &sb) == 0)
      kill(old_pid, SIGTERM);
  }

  // set new pid file
  return setPidFile();
} // end of 'Daemon::handlePidFile' function

bool Daemon::setPidFile()
{
  std::ofstream pid_file;
  pid_file.open(pidFileName, std::ofstream::out | std::ofstream::trunc);

  if (!pid_file.is_open())
  {
    syslog(LOG_ERR, "Couldn't open pid file");
    return false;
  }

  // write process pid
  pid_file << getpid();
  pid_file.close();

  return true;
} // end of 'Daemon::setPidFile' function

void Daemon::signalHandle( int sigNum )
{
  switch (sigNum)
  {
  case SIGHUP:
    if (!parseConfig())
      terminate();
    break;
  case SIGTERM:
    terminate();
  }
} // end of 'Daemon::signalHandle' function

void Daemon::terminate()
{
  // handle terminate signal
  clear();
  exit(SIGTERM);
} // end of 'Daemon::terminate' function

bool Daemon::parseConfig()
{
  syslog(LOG_INFO, "Open config");

  std::ifstream config_file(configFileName);
  if (!config_file.is_open())
  {
    syslog(LOG_ERR, "Couldn't open config file");
    clear();

    return false;
  }

  // Getting values from config
  std::map<ConfigParser::Parameter, string> params;
  bool success = ConfigParser::parse(config_file, params);
  config_file.close();

  string error_msg = "Config parsing error: %s";

  if (!success)
  {
    syslog(LOG_ERR, error_msg.c_str(), params.at(ConfigParser::Parameter::ERROR).c_str());
    clear();

    return false;
  }

  // extract time interval value
  if (params.find(ConfigParser::Parameter::TIME_INTERVAL) != params.end())
    try
    {
      long value = std::stol(params.at(ConfigParser::Parameter::TIME_INTERVAL));
      if (value < 0)
      {
        syslog(LOG_ERR, error_msg.c_str(), "negative time interval");
        return false;
      }

      timeInterval = static_cast<uint>(value);
    }
    catch (std::exception &e)
    {
      syslog(LOG_ERR, error_msg.c_str(), e.what());
      clear();

      return false;
    }

  // extract work directories names
  if (params.find(ConfigParser::Parameter::DIR1_NAME) != params.end())
    dir1Path = params.at(ConfigParser::Parameter::DIR1_NAME);
  if (params.find(ConfigParser::Parameter::DIR2_NAME) != params.end())
    dir2Path = params.at(ConfigParser::Parameter::DIR2_NAME);

  // get work directories absolute paths
  dir1Path = getAbsolutePath(dir1Path);
  dir2Path = getAbsolutePath(dir2Path);
  if (dir1Path.empty() || dir2Path.empty())
  {
    clear();
    return false;
  }

  // set up total.log path
  totalLogPath = dir2Path + "/total.log";

  syslog(LOG_INFO, "time interval: %d, dir1: %s, dir2: %s",
         timeInterval, dir1Path.c_str(), dir2Path.c_str());

  return true;
} // end of 'Daemon::parseConfig' function

void Daemon::startWorkLoop()
{
  // get pid file name
  #ifdef PID_FILE
    pidFileName = PID_FILE;
    syslog(LOG_INFO, "Used pid file name: %s", pidFileName.c_str());
  #endif
  getAbsolutePath(pidFileName);

  // handle pid file
  if (pidFileName.empty() || !handlePidFile())
    terminate();
  syslog(LOG_INFO, "pidFileName: %s", pidFileName.c_str());

  // do work in infinite loop
  syslog(LOG_INFO, "Start work loop");
  while (true)
  {
    doWork();
    sleep(timeInterval);
  }
} // end of 'Daemon::startWorkLoop' function

void Daemon::doWork()
{
  std::ofstream total_log_file;
  total_log_file.open(totalLogPath, std::ostream::out | std::ostream::app);

  if (total_log_file.is_open())
  {
    // find .log files
    svector log_files = findLogFiles(dir1Path);

    // save contents and delete .log files
    for (const string &log_path : log_files)
    {
      saveLog(log_path, total_log_file);
      unlink(log_path.c_str());
    }

    total_log_file.close();
  }
  else
    syslog(LOG_ERR, "Couldn't open '%s'", totalLogPath.c_str());
} // end of 'Daemon::doWork' function

Daemon::svector Daemon::findLogFiles( const string &curDir )
{
  //syslog(LOG_INFO, "Current search dir: %s", curDir.c_str());

  // open dir
  DIR *dir = opendir(curDir.c_str());
  if (dir == nullptr)
  {
    syslog(LOG_ERR, "Couldn't open '%s'", curDir.c_str());
    return svector();
  }

  // read dir contents
  svector output;
  struct dirent *entry;
  while ((entry = readdir(dir)) != nullptr)
    if (entry->d_type == DT_DIR)
    {
      // skip dots
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;

      // find .log files in child directory
      string child_dir_path = curDir + "/" + entry->d_name;
      svector child_logs = findLogFiles(child_dir_path);
      output.insert(output.end(), child_logs.begin(), child_logs.end());
    }
    else
    {
      string file_path = curDir + "/" + entry->d_name;

      // check if .log file
      string ext = file_path.substr(file_path.find_last_of(".") + 1);

      // save .log file name
      if (ext.compare("log") == 0)
        output.push_back(file_path);
    }

  closedir(dir);

  return output;
} // end of 'Daemon::findLogFiles' function

void Daemon::saveLog( const string &logFilePath, std::ofstream &totalLog )
{
  syslog(LOG_INFO, "Save .log file: %s", logFilePath.c_str());

  // open .log file
  std::ifstream log_file(logFilePath);
  if (!log_file.is_open())
  {
    syslog(LOG_ERR, "Couldn't open log file '%s'", logFilePath.c_str());
    return;
  }

  // read .log file contents
  string contents;
  log_file.seekg(0, std::ios::end);
  contents.resize(log_file.tellg());
  log_file.seekg(0, std::ios::beg);
  log_file.read(&contents[0], contents.size());
  log_file.close();

  // save .log file contents
  totalLog << logFilePath << "\n\n" << contents << "\n\n\n";
} // end of 'Daemon::saveLog' function

// END OF 'daemon.cpp' FILE
