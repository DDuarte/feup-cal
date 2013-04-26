#ifndef FILE_H_
#define FILE_H_

#include <string>
#include <functional>

#ifdef _WIN32
 #define WIN32_LEAN_AND_MEAN /* Needs this in order to compile in x64 Platform. Doesn't change anything in Win32 */
#include <windows.h>
#include "dirent.h" // our shipped file
#include <direct.h> // (_getcwd)
#define GetCurrentDir _getcwd
#elif
#include <dirent.h> // standard file
#include <unistd.h> // (getcwd)
#define GetCurrentDir getcwd
#endif

namespace File
{
    bool Load(const char* fileName, char*& buffer, size_t& size); ///< Loads a file to a buffer of bytes (char)
    bool Save(const char* fileName, const char* buffer, size_t size); ///< Saves a file from a buffer of bytes (char)
    bool Remove(const char* fileName); ///< Deletes file with the given file name
    bool Exists(const char* fileName); ///< Returns true if file exists
    template <class Container> bool GetFiles(const char* dirName, Container& container); ///< Puts file names of directory into container
    template <class Container> bool GetFiles(const char* dirName, Container& container, std::function<bool(const std::string&)> predicate); ///< Puts file names of directory into container that respect a predicate
}

template <class Container>
bool File::GetFiles(const char* dirName, Container& container)
{
    static_assert(std::is_convertible<Container::value_type, std::string>::value, "Container must contain std::string");

    if (!dirName)
        return false;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dirName)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
            if (ent->d_type == DT_REG) // file
                container.push_back(ent->d_name);

        closedir(dir);
        return true;
    }

    return false;
}


template <class Container>
bool File::GetFiles(const char* dirName, Container& container, std::function<bool(const std::string&)> predicate)
{
    std::vector<std::string> files;
    if (!File::GetFiles(dirName, files))
        return false;

    for (const std::string& file : files)
        if (predicate(file))
            container.push_back(file);

    return true;
}

#endif // FILE_H_
