// DirectoryFunctions.cpp

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#  include "win/dirent.h"
#else
#  include <unistd.h>
#  include <dirent.h>
//#  include <features.h>
#endif

#if defined(__APPLE__) || defined (_UNIX)
#include <sys/stat.h>
#endif

#include <algorithm>

#include "DirectoryFunctions.h"

// http://stackoverflow.com/questions/612097/how-can-i-get-a-list-of-files-in-a-directory-using-c-or-c
std::vector<std::string> GetListOfFilesFromDirectory(const std::string& d)
{
    std::vector<std::string> names;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (d.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            std::string s(ent->d_name);
#if defined(__APPLE__) || defined (_LINUX)
            std::string fullPath = d + s;
            struct stat st;
            stat( fullPath.c_str(), &st );
            if (S_ISDIR(st.st_mode))
                continue;
#else
            if (S_ISDIR(ent->d_type))
                continue;
#endif
            if (!s.compare("."))
                continue;
            if (!s.compare(".."))
                continue;

            names.push_back(s);
        }
        closedir(dir);
    }
    else
    {
        // could not open directory
    }

    std::sort(names.begin(), names.end());
    return names;
}

std::vector<std::string> GetListOfFilesFromDirectoryAndSubdirs(const std::string& d)
{
    std::vector<std::string> names;
    std::vector<std::string> directories;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (d.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            std::string s(ent->d_name);
            if (!s.compare("."))
                continue;
            if (!s.compare(".."))
                continue;
#if defined(__APPLE__) || defined (_LINUX)
            std::string fullPath = d + s;
            struct stat st;
            stat( fullPath.c_str(), &st );
            if (S_ISDIR(st.st_mode))
            {
                directories.push_back(s);
                continue;
            }
#else
            if (S_ISDIR(ent->d_type))
            {
                directories.push_back(s);
                continue;
            }
#endif
            names.push_back(s);
        }
        closedir(dir);
    }
    else
    {
        // could not open directory
    }

    // Scan all subdirectories(only one level deep, not recursion)
    for (std::vector<std::string>::const_iterator it = directories.begin();
        it != directories.end();
        ++it)
    {
        const std::string& subdir = *it;
        std::vector<std::string> dirFiles = GetListOfFilesFromDirectory(d + subdir);
        for (std::vector<std::string>::const_iterator it = dirFiles.begin();
            it != dirFiles.end();
            ++it)
        {
            const std::string& f = *it;
            const std::string filepath = subdir + "/" + f;
            names.push_back(filepath);
        }
    }

    std::sort(names.begin(), names.end());
    return names;
}
