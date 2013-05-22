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
  #include <unordered_map>
  template <typename K, typename T>
  class dict
  {
  public:
      typedef google::dense_hash_map<K, T> type_dense_hash_map;
      typedef std::unordered_map<K, T> type_unordered_map;

      static void init(type_dense_hash_map& d, K k) { d.set_empty_key(k); }
      static void init(type_unordered_map& d, K k) { }
  };
#else
  #include <unordered_map>
  template <typename K, typename T>
  class dict
  {
  public:
      typedef std::unordered_map<K, T> type_dense_hash_map;
      typedef std::unordered_map<K, T> type_unordered_map;
      static void init(type_dense_hash_map& d, K k) { }
      static void init(type_unordered_map& d, K k) { }
  };
#endif

#endif // UTILS_H_
