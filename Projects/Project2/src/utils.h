#ifndef UTILS_H_
#define UTILS_H_

#include <cstdint>
#include <string>

typedef int8_t int8; ///< Signed byte
typedef int16_t int16; ///< Signed short
typedef int32_t int32; ///< Signed int
typedef int64_t int64; ///< Signed 64 bits int
typedef uint8_t uint8; ///< Unsigned byte
typedef uint16_t uint16; ///< Unsigned short
typedef uint32_t uint32; ///< Unsigned 32 bits int
typedef uint64_t uint64; ///< Unsigned 64 bits int

// typedef uint32 uint; ///< Signed 32 bits int

typedef unsigned long ulong; ///< (System) Unsigned long
typedef unsigned int uint; ///< (System) Unsigned int
typedef unsigned short ushort; ///< (System) Unsigned short
typedef unsigned char uchar; ///< (System) Unsigned char

uint64 GetCurrentTime(); ///< Number of milliseconds representing time
uint32 GetTimeDiff(uint64 curTime, uint64 prevTime); ///< The time difference between 2 times, in milliseconds

void PauseConsole(const std::string& message = "Press enter to continue..."); ///< Prints a message and waits for a key to be pressed
void ClearConsole(); ///< Empties the console screen

bool starts_with(const std::string& str, const std::string& prefix); ///< Returns true if str starts with prefix
std::string to_lower(std::string str); // Converts a string to all lower case

/**
* Utilitie structure to reverse ranges of iterators
*/
template<class Fwd>
struct Reverser {
    const Fwd &fwd;
    Reverser<Fwd>(const Fwd& _fwd): fwd(_fwd) {}
    auto begin() -> decltype(fwd.rbegin()) const { return fwd.rbegin(); }
    auto end() -> decltype(fwd.rend()) const  { return fwd.rend(); }
};

template<class Fwd>
Reverser<Fwd> reverse(const Fwd& fwd) { return Reverser<Fwd>(fwd); }

#define USE_SPARSE_HASH 1

#if USE_SPARSE_HASH
#include <sparsehash/dense_hash_map>
template <typename K, typename T>
class dictionary
{
public:
    typedef google::dense_hash_map<K, T> type;
};
#define DICTIONARY_INIT(dict, val) (dict).set_empty_key((val))


typedef google::dense_hash_map<std::string, int> map_string_int;
typedef google::dense_hash_map<int, std::string> map_int_string;
typedef google::dense_hash_map<int, std::string> map_int_string;
typedef google::dense_hash_map<int, std::string> map_int_string;
#define MAP_STRING_INT_INIT(dict) dict.set_empty_key("")
#define MAP_INT_STRING_INIT(dict) dict.set_empty_key(-1)
#else
#include <unordered_map>
template <typename K, typename T>
class dictionary
{
public:
    typedef std::unordered_map<K, T> type;
};

#define DICTIONARY_INIT(dict, val)

typedef std::unordered_map<std::string, int>        map_string_int;
typedef std::unordered_map<int, std::string>        map_int_string;
typedef std::unordered_map<char, uint32>            map_char_uint;
#define MAP_STRING_INT_INIT(dict)
#define MAP_INT_STRING_INIT(dict)
#endif

#endif // UTILS_H_
