#include "ByteBuffer.h"
#include <vector>
#include <ostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <iomanip>
#include <cctype>
#include <malloc.h>

#ifdef _WIN32
#define alloca _alloca
#define snprintf sprintf_s
#endif

ByteBuffer::ByteBuffer(size_t capacity) : _readPos(0), _writePos(0), _bitPos(8), _curBitVal(0)
{
    _buffer.reserve(capacity);
}

ByteBuffer::ByteBuffer(const ByteBuffer& other)
    : _readPos(other._readPos), _writePos(other._writePos), _buffer(other._buffer), _bitPos(other._bitPos), _curBitVal(other._curBitVal) {}

ByteBuffer::ByteBuffer(ByteBuffer&& other)
    : _readPos(other._readPos), _writePos(other._writePos), _buffer(std::move(other._buffer)), _bitPos(other._bitPos), _curBitVal(other._curBitVal) { }

void ByteBuffer::Clear()
{
    _buffer.clear();
    _readPos = 0;
    _writePos = 0;
    _bitPos = 8;
    _curBitVal = 0;
}

void ByteBuffer::ReadSkip(size_t size)
{
    // assert(_readPos + size <= Size());

    if (_readPos + size > Size())
        throw ByteBufferException();

    _readPos += size;
}

void ByteBuffer::Read(Byte* dest, size_t count)
{
    // assert(_readPos + count <= Size());

    if (_readPos + count > Size())
        throw ByteBufferException();

    memcpy(dest, &_buffer[_readPos], count);
    _readPos += count;
}

void ByteBuffer::Append(const ByteBuffer& other)
{
    if (_writePos)
        Append(other.Data(), other._writePos);
}

void ByteBuffer::Append(const Byte* src, size_t count)
{
    // assert(count);
    // assert(src);
    // assert(Size() < 100000);

    if (!count || !src)
        throw ByteBufferException();

    if (Size() < _writePos + count)
        _buffer.resize(_writePos + count);
    memcpy(&_buffer[_writePos], src, count);
    _writePos += count;
}

void ByteBuffer::Put(size_t pos, const Byte* src, size_t count)
{
    // assert(pos + count <= Size());
    // assert(src);

    if (pos + count > Size() || !src)
        throw ByteBufferException();

    memcpy(&_buffer[pos], src, count);
}

void ByteBuffer::Resize(size_t newSize)
{
    _buffer.resize(newSize, 0);
    _readPos = 0;
    _writePos = Size();
}

void ByteBuffer::Reserve(size_t size)
{
    if (size > Size())
        _buffer.reserve(size);
}

void ByteBuffer::Print(std::ostream& stream) const
{
    const char* header = "|-------------------------------------------------|---------------------------------|\n"
        "| 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | 0 1 2 3 4 5 6 7 8 9 A B C D E F |\n"
        "|-------------------------------------------------|---------------------------------|\n";

    stream << header;

    std::stringstream text;
    std::stringstream hex;

    size_t length = Size();
    for (size_t i = 0; i < length; i += 16) // 16 bytes per line
    {
        hex << "| ";

        for (int j = 0; j < 16; ++j)
        {
            if (j + i < length)
            {
                Byte val = _buffer[j + i];
                char h[3]; // 2 chars + 1 null
                snprintf(h, sizeof(h), "%X", val); // convert byte to hex upper-case
                hex << std::setfill('0') << std::setw(2) << h << " ";

                // print all printable characters in the text side
                if (std::isgraph(val))
                    text << (char)val;
                else
                    text << '.';

                text << ' ';
            }
            else
            {
                hex << "   ";
                text << "  ";
            }
        }

        hex << "| " << text.str() << "|\n";
        stream << hex.str();

        // reset streams to be re-used
        hex.str("");
        hex.clear();
        text.str("");
        text.clear();
    }

    stream << "|-------------------------------------------------|---------------------------------|\n";
}

void ByteBuffer::WriteString(const std::string& value)
{
    Append7BitEncodedInt(length);
    if (length)
        Append((Byte const*)value.c_str(), length);
}

void ByteBuffer::WriteCString(const char* str)
{
    if (str)
        if (size_t length = strlen(str))
            Append((Byte const*)str, length);

    Append<char>(0); // null terminator
}

std::string ByteBuffer::ReadString()
{
    size_t length = Read7BitEncodedInt();
    // assert(length < 250000); // alloca is dangerous when allocating many bytes

    if (length >= 250000)
        throw ByteBufferException();

    Byte* res = (Byte*)alloca(length+1);
    Read(res, length);
    res[length] = 0; // null terminator
    std::string value = (const char*)res;

    return value;
}

std::string ByteBuffer::ReadCString()
{
    std::string value;
    value.reserve(50);

    while (GetWritePos() < Size())
    {
        char c = Read<char>();
        if (c == 0)
            break;

        value += c;
    }

    return value;
}

void ByteBuffer::Append7BitEncodedInt(size_t value)
{
    // assert(value <= 0xFFFFFFF); // 8*4 - 4*1 bits

    if (value > 0xFFFFFFF)
        throw ByteBufferException();

    while (value >= 0x80)
    {
        Append((Byte)(value | 0x80));
        value >>= 7;
    }
    Append((Byte)value);
}

size_t ByteBuffer::Read7BitEncodedInt()
{
    size_t result = 0;
    for (uint8 i = 0, shift = 0; i < 4; ++i, shift += 7) // read max 4 bytes (max value 0xFFFFFFF)
    {
        Byte val = Read<Byte>();
        Byte realVal = Byte(val & 0x7F);
        result |= realVal << shift;
        if (!(val & 0x80)) // if highbit is 1, continue reading otherwise stop
            break;
    }
    return result;
}

size_t ByteBuffer::SetBitWritePos(size_t newPos)
{
    _writePos = newPos / 8;
    _bitPos = 8 - (newPos % 8);
    return _writePos * 8 + 8 - _bitPos;
}

void ByteBuffer::FlushBits()
{
    if (_bitPos == 8)
        return;

    Append((Byte*)&_curBitVal, sizeof(Byte));
    _curBitVal = 0;
    _bitPos = 8;
}

bool ByteBuffer::WriteBit(uint32 bit)
{
    --_bitPos;
    if (bit)
        _curBitVal |= (1 << (_bitPos));

    if (_bitPos == 0)
        FlushBits();

    return bit != 0;
}

bool ByteBuffer::ReadBit()
{
    ++_bitPos;
    if (_bitPos > 7)
    {
        _bitPos = 0;
        _curBitVal = Read<Byte>();
    }

    return ((_curBitVal >> (7 - _bitPos)) & 1) != 0;
}

size_t ByteBuffer::ReadBits(size_t bits)
{
    size_t value = 0;
    for (ssize_t i = bits - 1; i >= 0; --i)
        if (ReadBit())
            value |= ((ssize_t)1 << i);

    return value;
}
