#include "generator_random_chars.h"
#include "bytebuffer.h"

#include <random>

GeneratorRandomChars::GeneratorRandomChars(uint charCount) : DataInput((int)charCount) { }

ByteBuffer GeneratorRandomChars::GenerateData() const 
{
    std::random_device generator;
    std::uniform_int_distribution<int> distribution(' ', '~'); // between 32 and 126

    ByteBuffer bb(GetMaxSize());

    for (int i = 0; i < GetMaxSize(); ++i)
        bb.WriteUInt8(distribution(generator));

    bb.WriteUInt8(0); // null terminator

    return bb;
}
