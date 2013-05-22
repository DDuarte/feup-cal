#include "compress_lzw.h"
#include "bytebuffer.h"

#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include "utils.h"

#define DEFAULT_DICTIONARY_SIZE 256

bool CompressLZW::CompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
    dict<std::string, int>::type_dense_hash_map dictionary;
    dict<std::string, int>::init(dictionary, "");

    int dictSize = DEFAULT_DICTIONARY_SIZE;
    for (int i = 0; i < DEFAULT_DICTIONARY_SIZE; i++)
        dictionary[std::string(1, i)] = i;

    const char* in = (const char*)input.Data();

    char c;
    std::string w, wc;
    dict<std::string, int>::type_dense_hash_map::iterator itr;
    for (size_t i = 0; i < input.Size(); ++i)
    {
        c = input[i];
        wc = w + c;

        itr = dictionary.find(wc);

        if (itr != dictionary.end())
            w = wc;
        else
        {
            output.WriteDynInt(dictionary[w]);
            dictionary.insert(itr, std::make_pair(wc, dictSize++));
            w = std::string(1, c);
        }
    }

    if (!w.empty())
        output.WriteDynInt(dictionary[w]);

    return true;
}

bool CompressLZW::DecompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
    dict<int, std::string>::type_dense_hash_map dictionary;
    dict<int, std::string>::init(dictionary, -1);

    int dictSize = DEFAULT_DICTIONARY_SIZE;
    for (int i = 0; i < dictSize; i++)
        dictionary[i] = std::string(1, i);

    ByteBuffer* hack = const_cast<ByteBuffer*>(&input);
    std::stringstream out;

    std::string w(1, hack->ReadDynInt());
    std::stringstream result;
    result << w;
    std::string entry;
    while (hack->CanRead())
    {
        int k = hack->ReadDynInt();
        if (dictionary.count(k))
            entry = dictionary[k];
        else if (k == dictSize)
            entry = w + w[0];
        else
            throw std::exception("The input file hasn't been compressed properly");

        result << entry;

        dictionary[dictSize++] = w + entry[0];

        w = entry;
    }

    std::string r = result.str();
    output.WriteBuffer(r.c_str(), r.size());
    //if (output.Data()[output.Size() - 1] != 0)
    //    output.WriteUInt8(0);
    return true;
}
