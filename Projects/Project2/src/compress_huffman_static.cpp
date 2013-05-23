#include "compress_huffman_static.h"
#include "bytebuffer.h"

#include <stdexcept>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <vector>

bool CompressHuffmanStatic::CompressImpl(const ByteBuffer& input1, ByteBuffer& output)
{
    typedef std::unordered_set<char> CharSet;
    typedef std::unordered_map<char, size_t> CharMap;
    typedef std::vector<char> CharVec;

    ByteBuffer& input = const_cast<ByteBuffer&>(input1);

    CharSet chars;

    size_t numberOfCharactersRead = 0;

    while (input.CanRead())
    {
        chars.insert(input.ReadInt8());
        numberOfCharactersRead++;
    }

    input.SetReadPos(input.GetReadPos() - numberOfCharactersRead);

    size_t numberOfCharacters = chars.size();
    size_t numberOfBits = (size_t)ceil(log<2>(static_cast<double>(numberOfCharacters)));

    CharMap charsIndex;
    CharVec charsOrdered(numberOfCharacters);
    size_t idx = 0;
    for (char c : chars)
    {
        charsOrdered[idx] = c;
        charsIndex[c] = idx++;
    }

    output.WriteDynInt(numberOfCharacters);

    for (char c : charsOrdered)
        output.WriteInt8(c);

    output.WriteDynInt(numberOfCharactersRead);

    while (input.CanRead())
    {
        char c = input.ReadInt8();
        output.WriteBits(charsIndex[c], numberOfBits);
    }

    output.FlushBits();

    return true;
}

bool CompressHuffmanStatic::DecompressImpl(const ByteBuffer& input1, ByteBuffer& output)
{
    typedef std::vector<char> CharVec;


    ByteBuffer& input = const_cast<ByteBuffer&>(input1);

    size_t numberOfCharacters = input.ReadDynInt();

    CharVec characters(numberOfCharacters);

    for (size_t i = 0; i < numberOfCharacters; ++i)
        characters[i] = input.ReadInt8();

    size_t numberOfBitsPerChar = (size_t)ceil(log<2>(static_cast<double>(numberOfCharacters)));
    size_t numberOfCharactersToRead = input.ReadDynInt();
    size_t totalNumberOfBits = numberOfBitsPerChar * numberOfCharactersToRead;

    for (int i = 0; i < numberOfCharactersToRead; ++i)
    {
        size_t coded = input.ReadBits(numberOfBitsPerChar);
        output.WriteInt8(characters[coded]);
    }

    return true;
}
