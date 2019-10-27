#include "Utils.h"

using namespace std;

void Utils::FileCopy(const string &from, const string &to)
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

bool Utils::FolderExists(const string &path)
{
    struct stat folder_stat;
    return stat(path.c_str(), &folder_stat) == 0 && S_ISDIR(folder_stat.st_mode);
}

void Utils::CreateFolder(const string &path)
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

DIR *Utils::OpenFolder(const string &path)
{
    DIR *folder = opendir(path.c_str());
    if (folder == nullptr)
    {
        syslog(LOG_ERR, "Failed to open directory %s", path.c_str());
        exit(EXIT_FAILURE);
    }
    return folder;
}

void Utils::ClearFolder(const string &path)
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