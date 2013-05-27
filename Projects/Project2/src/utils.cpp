#include "utils.h"
#include <chrono>
#include <cassert>
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <sstream>

// uint64 GetCurrentTime()
// {
//     return (uint64)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
// }
//
// uint32 GetTimeDiff(uint64 curTime, uint64 prevTime)
// {
//     assert(curTime >= prevTime);
//     return uint32(curTime - prevTime);
// }

void ClearConsole()
{
#ifdef _WIN32 // Windows: Console command (alternative: Windows API)
    system("cls");
#else // UNIX: ANSI escape codes
    std::cout << "\x1B[2J\x1B[H";
#endif
}

void PauseConsole(const std::string& message/* = "Press enter to continue..."*/)
{
    std::cout << message << std::endl;
    std::cin.get();

    std::cin.clear();

    if (std::cin.rdbuf()->in_avail() != 0)
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool starts_with(const std::string& str, const std::string& prefix)
{
    return !str.compare(0, prefix.size(), prefix);
}

bool ends_with(const std::string& str, const std::string& suffix)
{
    std::cout << "str: " << str << " suffix: " << suffix << std::endl;
    return !str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

bool contains(const std::string& str, const std::string fix)
{
    return str.find(fix) != std::string::npos;
}

std::string to_lower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string BytesPrettyPrint(uint bytes)
{
    const char* suffixes[7] = { "B", "KB", "MB", "GB", "TB", "PB", "EB" };
    uint s = 0; // which suffix to use
    double count = bytes;
    while (count >= 1024 && s++ < 7)
    {
        count /= 1024;
    }

    std::ostringstream ss;
    if (count - floor(count) == 0.0)
        ss << (int)count << ' ' << suffixes[s];
    else
        ss << std::setprecision(2) << count << ' ' << suffixes[s];
    return ss.str();
}
