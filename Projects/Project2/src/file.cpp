#include "file.h"
#include <iostream>
#include <cstdio>
#include <memory>

namespace File
{
    bool Load(const char* fileName, char*& buffer, size_t& size)
    {
        if (!fileName)
            return false;

        FILE* file;
        fopen_s(&file, fileName, "rb");
        if (!file)
        {
            std::cerr << "File::Load: Could not open file " << fileName << " (fopen)" << std::endl;
            return false;
        }

        // Get file size
        fseek(file, 0, SEEK_END);
        size = (size_t)ftell(file);
        rewind(file);

        if (!size)
        {
            std::cerr << "File::Load: Could not open file " << fileName << " (size is 0)" << std::endl;
            fclose(file);
            return false;
        }

        buffer = new char[size];
        if (!buffer)
        {
            std::cerr << "File::Load: Failed to allocate buffer for file " << fileName << " (size " << size << ")" << std::endl;
            fclose(file);
            return false;
        }

        size_t result = fread(buffer, sizeof(char), size, file);
        if (result != size)
        {
            std::cerr << "File::Load: Could not read the same number of bytes (size != result) for file " << fileName << " (size " << size << ", result " << result << ")" << std::endl;
            delete[] buffer;
            fclose(file);
            return false;
        }

        fclose(file);
        return true;
    }

    bool Save(const char* fileName, const char* buffer, size_t size)
    {
        if (!fileName || !buffer || !size)
            return false;

        FILE* file;
        fopen_s(&file, fileName, "wb");
        if (!file)
        {
            std::cerr << "File::Save: Could not open file " << fileName << " (fopen)" << std::endl;
            return false;
        }

        size_t result = fwrite(buffer, sizeof(char), size, file);
        if (result != size)
        {
            fclose(file);
            std::cerr << "File::Save: Could not write the same number of bytes (size != result) for file " << fileName << " (size " << size << ", result " << result << ")" << std::endl;
            return false;
        }

        fclose(file);
        return true;
    }

    bool Remove(const char* fileName)
    {
        return !remove(fileName);
    }

    bool Exists(const char* fileName)
    {
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(".")) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
                if (ent->d_type == DT_REG) // file
                    if (strcmp(ent->d_name, fileName) == 0)
                    {
                        closedir(dir);
                        return true;
                    }

            closedir(dir);
        }

        return false;
    }
}
