#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include <vector>
#include <ostream>
// #include <cassert>
#include <stdexcept>
#include "utils.h"

typedef uint8 Byte; ///< Alias for uint8 [0-256, 8 bits, 1 byte]

/// Exception thrown when something bad happens in the ByteBuffer
class ByteBufferException : public std::exception {};

//! ByteBuffer Class
/*!
    ByteBuffer is a container of bytes.
    It allows to represent multiple types (ints, strings, floats, etc) in
     a binary format (great for any form of communication (files, network, etc.))
*/
class ByteBuffer
{
public:
    ByteBuffer(size_t capacity); ///< Constructor. capacity is a raw guess for the initial size of the buffer
    ByteBuffer(const ByteBuffer& other); ///< Copy constructor
    ByteBuffer(ByteBuffer&& other);

    const Byte* Data() const { return &_buffer[0]; } ///< Underlying Byte array

    void Clear(); ///< Empties the buffer

    Byte operator[](size_t pos) const { return Read<Byte>(pos); } ///< Indexer, performs a read
    template <typename T> T Read(size_t position) const; ///< Read a value of type T at a given position

    size_t GetReadPos() const { return _readPos; } ///< Returns the current read position
    void SetReadPos(size_t readPos) { _readPos = readPos; } ///< Updates the current read position

    size_t GetWritePos() const { return _writePos; } ///< Returns the current write position
    void SetWritePos(size_t writePos) { _writePos = writePos; } ///< Updates the current write position

    void FinishRead() { _readPos = _writePos; } ///< Advances read position to end of buffer, ignoring all its content

    template <typename T> void ReadSkip() { ReadSkip(sizeof(T)); } ///< Advances read position by sizeof(T), ignoring those Bytes
    void ReadSkip(size_t size); ///< Advances read position by the given size (in Bytes), ignoring those Bytes

    size_t Size() const { return _buffer.size(); } ///< Returns the size of the buffer
    bool IsEmpty() const { _buffer.empty(); } ///< True if buffer is currently empty

    // bit related methods
    void FlushBits();
    bool WriteBit(uint32 bit);
    bool ReadBit();
    template <typename T> void WriteBits(T value, size_t bits);
    size_t ReadBits(size_t bits);
    template <typename T> void PutBits(size_t pos, T value, uint32 bitCount);
    size_t GetBitWritePos() { return _writePos * 8 + 8 - _bitPos; }
    size_t SetBitWritePos(size_t newPos);

    void Resize(size_t newSize); ///< Resizes the underlying buffer to a new size (adding (nulls) or removing content)
    void Reserve(size_t size); ///< Reserves space for the underlying buffer (see std::vector<T>::reserve)

    bool CanRead() const { return _readPos < (_buffer.size()); }

    void Print(std::ostream& stream) const; ///< Prints the buffer content in an user friendly way (both ascii and hex)

    void WriteBool(bool value) { Append<bool>(value); }
    void WriteUInt8(uint8 value) { Append<uint8>(value); }
    void WriteUInt16(uint16 value) { Append<uint16>(value); }
    void WriteUInt32(uint32 value) { Append<uint32>(value); }
    void WriteUInt64(uint64 value) { Append<uint64>(value); }
    void WriteInt8(int8 value) { Append<int8>(value); }
    void WriteInt16(int16 value) { Append<int16>(value); }
    void WriteInt32(int32 value) { Append<int32>(value); }
    void WriteInt64(int64 value) { Append<int64>(value); }
    void WriteFloat(float value) { Append<float>(value); }
    void WriteDouble(double value) { Append<double>(value); }
    void WriteBuffer(const ByteBuffer& value) { Append(value); }
    void WriteString(const std::string& value); ///< Writes a variable length size (4 bytes max) then the string
    void WriteString(const char* value) { WriteString(std::string(value)); }
    void WriteCString(const char* value); ///< Writes a null terminated string
    void WriteCString(const std::string& value) { WriteCString(value.c_str()); }
    void WriteBuffer(const char* src, size_t count) { Append(src, count); }
    void WriteDynInt(size_t val) { Append7BitEncodedInt(val); }

    bool ReadBool() { return Read<uint8>() > 0 ? true : false; }
    uint8 ReadUInt8() { return Read<uint8>(); }
    uint16 ReadUInt16() { return Read<uint16>(); }
    uint32 ReadUInt32() { return Read<uint32>(); }
    uint64 ReadUInt64() { return Read<uint64>(); }
    int8 ReadInt8() { return Read<int8>(); }
    int16 ReadInt16() { return Read<int16>(); }
    int32 ReadInt32() { return Read<int32>(); }
    int64 ReadInt64() { return Read<int64>(); }
    float ReadFloat() { return Read<float>(); }
    double ReadDouble() { return Read<double>(); }
    std::string ReadString(); ///< Reads a string prefixed with a variable length size (4 bytes max)
    std::string ReadCString(); ///< Reads a string until the null terminator is found
    size_t ReadDynInt() { return Read7BitEncodedInt(); }

    ByteBuffer& operator <<(bool value) { WriteBool(value); return *this; }
    ByteBuffer& operator <<(uint8 value) { WriteUInt8(value); return *this; }
    ByteBuffer& operator <<(uint16 value) { WriteUInt16(value); return *this; }
    ByteBuffer& operator <<(uint32 value) { WriteUInt32(value); return *this; }
    ByteBuffer& operator <<(uint64 value) { WriteUInt64(value); return *this; }
    ByteBuffer& operator <<(int8 value) { WriteInt8(value); return *this; }
    ByteBuffer& operator <<(int16 value) { WriteInt16(value); return *this; }
    ByteBuffer& operator <<(int32 value) { WriteInt32(value); return *this; }
    ByteBuffer& operator <<(int64 value) { WriteInt64(value); return *this; }
    ByteBuffer& operator <<(float value) { WriteFloat(value); return *this; }
    ByteBuffer& operator <<(double value) { WriteDouble(value); return *this; }
    ByteBuffer& operator <<(const std::string& value) { WriteString(value); return *this; }
    ByteBuffer& operator <<(const char* value) { WriteCString(value); return *this; }
    ByteBuffer& operator <<(const ByteBuffer& value) { WriteBuffer (value); return *this; }

    ByteBuffer& operator >>(bool& value) { value = ReadBool(); return *this; }
    ByteBuffer& operator >>(uint8& value) { value = ReadUInt8(); return *this; }
    ByteBuffer& operator >>(uint16& value) { value = ReadUInt16(); return *this; }
    ByteBuffer& operator >>(uint32& value) { value = ReadUInt32(); return *this; }
    ByteBuffer& operator >>(uint64& value) { value = ReadUInt64(); return *this; }
    ByteBuffer& operator >>(int8& value) { value = ReadInt8(); return *this; }
    ByteBuffer& operator >>(int16& value) { value = ReadInt16(); return *this; }
    ByteBuffer& operator >>(int32& value) { value = ReadInt32(); return *this; }
    ByteBuffer& operator >>(int64& value) { value = ReadInt64(); return *this; }
    ByteBuffer& operator >>(float& value) { value = ReadFloat(); return *this; }
    ByteBuffer& operator >>(double& value) { value = ReadDouble(); return *this; }
    ByteBuffer& operator >>(std::string& value) { value = ReadString(); return *this; }

    operator const char*() { return (const char*)Data(); } ///< Implicit conversion to char*
    operator const std::string() { return std::string(_buffer.begin(), _buffer.end()); } ///< Implicit conversion to std::string

protected:
    std::vector<Byte> _buffer; ///< Underlying buffer of Bytes
    size_t _readPos; ///< Current read position
    size_t _writePos; ///< Current write position
    size_t _bitPos; ///< Current bit position [0, 8]
    uint8 _curBitVal; ///< Current bit value [0, 255]

    // Use stream operators to read and write
    template <typename T> T Read();
    void Read(Byte* dest, size_t count);
    template <typename T> void Append(T val);
    template <typename T> void Append(const T* src, size_t count);
    void Append(const ByteBuffer& other);
    void Append(const Byte* src, size_t count);
    template <typename T> void Put(size_t pos, T val) { Put(pos, (Byte*)&val, sizeof(T)); }
    void Put(size_t pos, const Byte* src, size_t count);

private:
    void Append7BitEncodedInt(size_t value); ///< Variable length value
    size_t Read7BitEncodedInt(); ///< Variable length value
};

template <typename T>
void ByteBuffer::PutBits(size_t pos, T value, uint32 bitCount)
{
    if (!bitCount)
        throw ByteBufferException(/* (pos + bitCount) / 8, size(), 0 */);

    if (pos + bitCount > size() * 8)
        throw ByteBufferException(/* false, (pos + bitCount) / 8, size(), (bitCount - 1) / 8 + 1 */);

    for (uint32 i = 0; i < bitCount; ++i)
    {
        size_t wp = (pos + i) / 8;
        size_t bit = (pos + i) % 8;
        if ((value >> (bitCount - i - 1)) & 1)
            _buffer[wp] |= 1 << (7 - bit);
        else
            _buffer[wp] &= ~(1 << (7 - bit));
    }
}

template <typename T>
void ByteBuffer::WriteBits(T value, size_t bits)
{
    for (int32 i = bits - 1; i >= 0; --i)
        WriteBit((value) & 1 << i);
}

template <typename T>
T ByteBuffer::Read(size_t position) const
{
    //assert(position + sizeof(T) <= Size());

    if (position + sizeof(T) > Size())
        throw ByteBufferException();

    T value = *((T const*)&_buffer[position]);
    return value;
}

template <typename T>
T ByteBuffer::Read()
{
    T val = Read<T>(_readPos);
    _readPos += sizeof(T);
    return val;
}

template <typename T>
void ByteBuffer::Append(T val)
{
    FlushBits();
    Append((Byte*)&val, sizeof(T));
}

template <typename T>
void ByteBuffer::Append(const T* src, size_t count)
{
    return Append((const Byte*)src, count * sizeof(T));
}

#endif // BYTEBUFFER_H
