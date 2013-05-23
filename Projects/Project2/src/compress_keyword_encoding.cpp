#include "compress_keyword_encoding.h"
#include "bytebuffer.h"

#include <stdexcept>
#include <sstream>
#include <map>
#include <unordered_map>

#include <iostream>
#include <regex>
#include <cctype>
#include <algorithm>

#include "utils.h"

std::vector<uint8> InitializeSymbols()
{
    std::vector<uint8> vec;
    for (uint8 i = 0; i < 255; ++i)
        if (std::isprint(i) && i != '$') // $ is used by regex replace
            vec.push_back(i);

    return vec;
}

bool CompressKeywordEncoding::CompressImpl(const ByteBuffer& input, ByteBuffer& output)
{

    // regex is used to match words and build frequency map

    std::cmatch sm;
    std::regex e("\\b\\S+\\b"); // word

    dict<std::string, unsigned int>::type_unordered_map stringFrequencies;
    dict<std::string, unsigned int>::init(stringFrequencies, "");

    {
        std::string str((const char*)input.Data(), input.Size());
        while (std::regex_search(str.c_str(), sm, e))
        {
            for (auto x : sm)
                stringFrequencies[x.str()]++;
            str = sm.suffix().str();
        }
    }

    // remove used symbols

    std::vector<uint8> symbols = InitializeSymbols();
    for (size_t i = 0; i < input.Size(); ++i)
        std::remove(symbols.begin(), symbols.end(), input.Data()[i]);

    if (symbols.empty())
        std::cerr << "Empty symbols!!" << std::endl;

    // invert frequency map, key by frequency

    std::map<unsigned int, std::vector<std::string>> reverseStringFrequencies;
    std::map<unsigned int, std::vector<std::string>>::iterator it1;

    for (const auto& elem: stringFrequencies)
    {
        it1 = reverseStringFrequencies.find(elem.second);
        if (it1 == reverseStringFrequencies.end())
            it1 = reverseStringFrequencies.insert(it1, std::make_pair(elem.second, std::vector<std::string>()));

        it1->second.push_back(elem.first);
    }

    // pick symbols for common words

    std::unordered_map<std::string, uint8> encodedStrings;
    std::unordered_map<std::string, uint8>::iterator encodedStringsIt;
    std::map<unsigned int, std::vector<std::string>>::reverse_iterator it2 = reverseStringFrequencies.rbegin();
    for (size_t i = 0; i < symbols.size() && it2 != reverseStringFrequencies.rend(); ++it2)
    {
        for (size_t j = 0; j < it2->second.size() && i < symbols.size(); ++j)
        {
            //if ((it2->second[j].size() > 2 /* min word size */ && it2->first > 1 /* min freq */) ||
            //    (it2->second[j].size() > 1 /* min word size */ && it2->first > 2 /* min freq */))
            {
                encodedStrings.insert(std::make_pair(it2->second[j], symbols[i++]));
            }
        }
    }

    // write symbol - word map

    output.WriteDynInt(encodedStrings.size());
    for (encodedStringsIt = encodedStrings.begin(); encodedStringsIt != encodedStrings.end(); ++encodedStringsIt)
    {
        output.WriteUInt8(encodedStringsIt->second);
        output.WriteString(encodedStringsIt->first);
    }

    // modify and write text

    {
        std::string str((const char*)input.Data(), input.Size());

        for (std::unordered_map<std::string, uint8>::value_type p : encodedStrings)
        {
            std::regex rx("\\b" + p.first + "\\b");
            str = std::regex_replace(str, rx, std::string(1, p.second));
        }

        output.WriteBuffer(str.data(), str.size());
    }

    return true;
}

bool CompressKeywordEncoding::DecompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
    ByteBuffer* hack = const_cast<ByteBuffer*>(&input);

    std::unordered_map<uint8, std::string> encodedSymbols; // symbol -> word

    size_t numTableEntries = hack->ReadDynInt();
    for(; numTableEntries > 0; --numTableEntries)
    {
        uint8 c = hack->ReadUInt8();
        std::string str = hack->ReadString();
        encodedSymbols[c] = str;
    }

    std::string str(&(*((const char *)input.Data() + hack->GetReadPos())), input.Size() - hack->GetReadPos());

    std::unordered_map<uint8, std::string>::const_iterator itr;
    while (hack->CanRead())
    {
        uint8 c = hack->ReadUInt8();

        itr = encodedSymbols.find(c);
        if (itr != encodedSymbols.end())
            output.WriteBuffer(itr->second.data(), itr->second.size());
        else
            output.WriteUInt8(c);
    }

    return true;
}
