#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <ftw.h>
#include <syslog.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <dirent.h>
#include <signal.h>

std::string PID_FILENAME = "pid_info.txt";
std::string PROC_DIR = "/proc";
int WAIT_OF_KILLING_TIME = 1;

std::string work_dir;
std::string config_path;
std::string src_dir;
std::string dst_dir;
std::string pid_file_path;
int update_freq;
bool is_need_work;

void delete_pid_file(){
    unlink(pid_file_path.c_str());
}

int UnlinkCB (const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf) {
  int rv;

  if (ftwbuf->level == 0) {
    return 0;
  }
  rv = remove(fpath);
  if (rv) {
    //perror(fpath);
    syslog(LOG_ERR, "Something goes wrong while deleating files \"%s\"", fpath);
    //std::cout << "Something goes wrong while deleating files \"" << fpath <<"\"\n";
    delete_pid_file();
    exit(EXIT_FAILURE);
  }  
  return rv;
}


void ClearDirectory (const std::string& absPath) {
  nftw(absPath.c_str(), UnlinkCB, 64, FTW_DEPTH | FTW_PHYS); 
}


void CreateDirectory (const std::string& absPath, const std::string& name){
   std::string currentPath(get_current_dir_name()); 
   int error;
   error = chdir(absPath.c_str());
   if (error) {
      syslog(LOG_ERR, "Could not change directory \"%s\"", absPath.c_str());
      //std::cout << "Could not change directory \"" << absPath << "\"" << std::endl;
      delete_pid_file();
      exit(EXIT_FAILURE);
   }  
   error = mkdir(name.c_str(), S_IRWXU);
   if (error) {
      syslog(LOG_ERR, "Could not create directory \"%s\"", absPath.c_str());
      //std::cout << "Could not create directory \"" << absPath << "\"" << std::endl;
      delete_pid_file();
      exit(EXIT_FAILURE);
   }     
   chdir(currentPath.c_str());
}

void CreateDaemon(const std::string configFileName){
  work_dir = std::string(get_current_dir_name());
  config_path = work_dir + "/" + configFileName;
  pid_file_path = work_dir +"/" + PID_FILENAME;
  is_need_work = true;
  //std::cout << "WORK_DIR\t" << work_dir << "\n";
  //std::cout << "CONFIG_FILE\t" << config_path << "\n";
  //std::cout << "PID_FILE\t" << pid_file_path << "\n";
}


void LoadConfigFile(){
  std::ifstream cfg_file(config_path.c_str());
  if (!cfg_file.is_open() || cfg_file.eof()) {
    syslog(LOG_ERR, "Could not open config file \"%s\" or it is empty", config_path.c_str());
    exit(EXIT_FAILURE);
  }
  cfg_file >> src_dir >> dst_dir >> update_freq;
  cfg_file.close();
  if(src_dir == dst_dir){
    syslog(LOG_ERR, "destination and source folders can not be the same (%s)", src_dir.c_str());
    exit(EXIT_FAILURE);
  }
 if(update_freq <= 0){
    syslog(LOG_ERR, "frequency time must be greater then zero (current:%i)", update_freq);
    exit(EXIT_FAILURE);
  }
  //std::cout << "src_dir\t" << src_dir << "\n";
  //std::cout << "dst_dir\t" << dst_dir << "\n";
  //std::cout << "update_freq\t" << update_freq << "\n";
}


void signalHandler(int signal){
  switch (signal) {
    case SIGHUP:{ 
      is_need_work = false;
      syslog(LOG_USER, "Reload daemon's config by signal");
      //std::cout << "Reload daemon's config by signal\n";
      LoadConfigFile();
      is_need_work = true;
      break;
    }
    case SIGTERM:{
      is_need_work = false;
      syslog(LOG_USER, "Terminate daemon by signal");
      //std::cout << "Terminate daemon by signal \n";
      delete_pid_file();
      closelog();
      exit(EXIT_SUCCESS);
      break;
    } 
  }
}


void InitPidFile () {
   std::ofstream f(pid_file_path.c_str());
   syslog(LOG_USER, "pid file created: %s, pid is %i", pid_file_path.c_str(), getpid());  
   //std::cout << "pid file created: " << pid_file_path.c_str() << ", pid is " << getpid() << std::endl;
   f << getpid() << std::endl; 
}


std::string ReadPidFile () {
   std::fstream f(pid_file_path.c_str());
   if (f.good()) {
      std::string line;
      getline(f, line);
      return line;
   }   
   return std::string(""); 
}


void KillPrevDaemon() {
  DIR*   dir;
  std::string pid = ReadPidFile();
  std::string pidFolder = (std::string(PROC_DIR) + "/" + pid);
  //std::cout << "pidFolder\t" << pidFolder;
  if (pid == "") {
    return;
  }

  syslog(LOG_USER, "Deamon already works - kill him");
  //std::cout << "Deamon already works - kill him\n";
  if ((dir = opendir(pidFolder.c_str())) != nullptr) {
    kill(atoi(pid.c_str()), SIGTERM);  
  }
  while ((dir = opendir(pidFolder.c_str())) != nullptr) {
    sleep(WAIT_OF_KILLING_TIME);
  }
}


void Init(){
  int error;
  //std::cout << "Init\n";

  int pid = fork();
  if (pid == -1) {
    //std::cout << "Error: start daemon failed" << std::endl;
    syslog(LOG_ERR, "Could not fork()");
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  umask(0);
  error = setsid();

  if(error == -1){
    syslog(LOG_ERR, "Could not creat a new session");
    exit(EXIT_FAILURE);
  }

  pid = fork();
  if (pid == -1) {
    //std::cout << "ECould not fork" << std::endl;
    syslog(LOG_ERR, "Could not fork()");
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  error = chdir("/");
  if (error < 0){
    syslog(LOG_ERR, "Could not change working directory to /");
    exit(EXIT_FAILURE);
  }

  KillPrevDaemon();
  InitPidFile();

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  signal(SIGHUP, signalHandler);   
  signal(SIGTERM, signalHandler);
}


void CopyFile (const std::string& absPathSrc, const std::string& absPathDst){
  std::ifstream src(absPathSrc.c_str(), std::ios::binary);
  if (!src.good()) {
    //std::cout << "Copy file goes bad: src = " << absPathSrc << "\tdst = " << absPathDst << std::endl;
    syslog(LOG_ERR, "Copy file goes bad src = %s\tdst = %s", absPathSrc.c_str(), absPathDst.c_str());
    return;
  }   
  std::ofstream dst(absPathDst.c_str(), std::ios::binary);
  dst << src.rdbuf();
}


bool CompareExtention(const std::string fileName, const std::string extention){
  if(fileName.size() <= extention.size()){
    return false;
  }
  int startInd = fileName.size() - extention.size();
  for(unsigned int i = 0; i < extention.size(); i++){
    if(fileName[startInd + i] != extention[i]){
      return false;
    }
  }
  return true;   
} 


std::vector<std::string> GetContentList (const std::string& absPath){
  DIR*           dir;
  struct dirent* ent;
  std::vector<std::string> res;
  if ((dir = opendir(absPath.c_str())) != nullptr) {
    while ((ent = readdir(dir)) != nullptr) {
      if (ent->d_type == DT_DIR) {
        continue;
      }
      //std::cout << ent->d_name << std::endl;
      res.push_back(ent->d_name);
    }
    closedir(dir);
  } else {
    return std::vector<std::string>();
  }
  return res;
}


void DoWork(){
  //std::cout << "DoWork\n";
  ClearDirectory(dst_dir);
  //std::cout << "dst_dir\t" << dst_dir << "\n";
  CreateDirectory(dst_dir, "IMG");
  CreateDirectory(dst_dir, "OTHERS");
  std::vector<std::string> srcContent = GetContentList(src_dir);
  std::string filename;
  std::string srcPath;
  std::string dstPath;
  for(unsigned int i = 0; i < srcContent.size(); i++){
    filename = srcContent[i];
    srcPath = src_dir + "/" + filename;
    if(CompareExtention(filename, ".png")){
      dstPath = dst_dir + "/IMG/" + filename;
    } else {
      dstPath = dst_dir + "/OTHERS/" + filename;
    }
    CopyFile(srcPath, dstPath);      
  }

}


void Work(){
  //std::cout << "Work\n";
  while(true){
    if (is_need_work) {
      DoWork();
    }
    sleep(update_freq);   
  }
}


int main (int argc, char** argv){
  if (argc != 2) {
    std::cout << "Bad arguments ( args != 2 )" << std::endl;
    return -1;
  }
  openlog("daemon_lab", 0, LOG_USER);
  std::string cfgFileName(argv[1]); 

  CreateDaemon(cfgFileName);
  LoadConfigFile();
  Init();
  Work();
  return 0;
}
