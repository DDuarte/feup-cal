#include "compress_keyword_encoding.h"
#include "bytebuffer.h"

#include <stdexcept>
#include <sstream>
#include <map>
#include <unordered_map>

#include <iostream>

std::string initializeSymbols()
{
    std::string s;
    for(int i = 1; i < 255; ++i)
        s += i;

    return s;
}

bool CompressKeywordEncoding::CompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
    std::string symbols = initializeSymbols();

    const char * inChars = (const char *)input.Data();

    size_t symIndex;
    char c;
    for (size_t i=0; i < input.Size(); ++i)
    {
        c = inChars[i];

        if ((symIndex = symbols.find(c)) != std::string::npos)
            symbols.erase(symbols.begin() + symIndex);
    }

    std::map<std::string, unsigned int>::iterator it;
    std::map<std::string, unsigned int> stringFrequencies;

    std::istringstream in((char *)input.Data());
    std::string temp;
    while (in.good())
    {
        in >> temp;
        it = stringFrequencies.find(temp);

        if (it != stringFrequencies.end())
            ++it->second;
        else
            stringFrequencies.insert(it, std::make_pair(temp, 1));
    }

    std::map<unsigned int, std::vector<std::string>> reverseStringFrequencies;
    std::map<unsigned int, std::vector<std::string>>::iterator it1;

    for (const auto& elem: stringFrequencies)
    {
        it1 = reverseStringFrequencies.find(elem.second);
        if (it1 == reverseStringFrequencies.end())
            it1 = reverseStringFrequencies.insert(it1, std::make_pair(elem.second, std::vector<std::string>()));

        it1->second.push_back(elem.first);
    }

    std::unordered_map<std::string, char> encodedStrings;
    std::unordered_map<std::string, char>::iterator encodedStringsIt;

    std::map<unsigned int, std::vector<std::string>>::reverse_iterator it2 = reverseStringFrequencies.rbegin();
    for (size_t i = 0; i < symbols.size() && it2 != reverseStringFrequencies.rend(); ++it2)
    {
        for (size_t j = 0; j < it2->second.size() && i < symbols.size(); ++j)
        {
            if ((it2->second[j].size() > 2 /* min word size */ && it2->first > 1 /* min freq */) ||
                (it2->second[j].size() > 1 /* min word size */ && it2->first > 2 /* min freq */))
            {
                encodedStrings.insert(std::make_pair(it2->second[j], symbols[i++]));
                std::cout << it2->second[j] << std::endl;
            }
        }
    }

    output.WriteDynInt(encodedStrings.size());
    for(encodedStringsIt = encodedStrings.begin(); encodedStringsIt != encodedStrings.end(); ++encodedStringsIt)
    {
        output.WriteUInt8(encodedStringsIt->second);
        output.WriteString(encodedStringsIt->first);
    }

    in = std::istringstream(std::string((const char *)input.Data(), input.Size()));

    std::string str = in.str();

    std::ostringstream out;

    while (in.good())
    {
        std::streamoff prev = in.tellg();
        in >> temp;
        std::streamoff after = in.tellg();

        if ((encodedStringsIt = encodedStrings.find(temp)) != encodedStrings.end())
            output.WriteString(std::string(1, encodedStringsIt->second));
        else
            output.WriteString(temp);

        std::string str2;
        if (after != -1)
            output.WriteString((str2 = str.substr(prev, after - prev - temp.size())));
        else
            output.WriteString(str.substr(prev, input.Size() - prev - str2.size()));
    }

    return true;
}

bool CompressKeywordEncoding::DecompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
    ByteBuffer* hack = const_cast<ByteBuffer*>(&input);

    std::unordered_map<char, std::string> encodedSymbols;
    std::unordered_map<char, std::string>::iterator encodedSymbolsIt;

    size_t numTableEntries = hack->ReadDynInt();
    for(; numTableEntries > 0; --numTableEntries)
        encodedSymbols.insert(std::make_pair((char)hack->ReadUInt8(), hack->ReadString()));

    std::stringstream ss;

    std::string temp, spaces;
    while (hack->CanRead())
    {
        temp = hack->ReadString();
        spaces = hack->ReadString();

        if (hack->CanRead())
        {
            ss << spaces;

            if (temp.size() > 0 && (encodedSymbolsIt = encodedSymbols.find(temp[0])) != encodedSymbols.end())
                ss << encodedSymbolsIt->second;
            else
                ss << temp;
        }

        if (!hack->CanRead()) // last iter
            ss << spaces;
    };

    std::string r = ss.str();
    output.WriteBuffer(r.c_str(), r.size());
    //output.WriteUInt8(0);

    return true;
}
