#include "compress_keyword_encoding.h"
#include "bytebuffer.h"

#include <stdexcept>
#include <sstream>
#include <map>
#include <unordered_map>

#include <iostream>

std::string  initializeSymbols()
{
	std::string s;
	for(int i = 0; i < 127; ++i)
		s += i;

	return s;
}

bool CompressKeywordEncoding::CompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
    std::string symbols = initializeSymbols();

	const char * inChars = (const char *)input.Data();

	size_t symIndex;
	char c;
	for(size_t i=0; i < input.Size(); ++i)
	{
		c = inChars[i];

		if((symIndex = symbols.find(c)) != std::string::npos)
			symbols.erase(symbols.begin() + symIndex);
	}

	std::map<std::string, unsigned int>::iterator it;
	std::map<std::string, unsigned int> stringFrequencies;

	std::istringstream in((char *)input.Data());
	std::string temp;
	while(in.good())
	{
		in >> temp;
		it = stringFrequencies.find(temp);

		if(it != stringFrequencies.end())
			++it->second;
		else
			stringFrequencies.insert(it, std::make_pair(temp, 1));
	}

	std::map<unsigned int, std::vector<std::string>> reverseStringFrequencies;
	std::map<unsigned int, std::vector<std::string>>::iterator it1;

	for (const auto& elem: stringFrequencies)
	{
		it1 = reverseStringFrequencies.find(elem.second);
		if (it1 != reverseStringFrequencies.end())
		{
			it1->second.push_back(elem.first);
		}
		else
		{
			it1 = reverseStringFrequencies.insert(it1, std::make_pair(elem.second, std::vector<std::string>()));
			it1->second.push_back(elem.first);
		}
	}

	std::unordered_map<std::string, char> encodedStrings;
	std::unordered_map<std::string, char>::iterator encodedStringsIt;

	std::map<unsigned int, std::vector<std::string>>::reverse_iterator it2 = reverseStringFrequencies.rbegin();
    for (size_t i = 0; i < symbols.size() && it2 != reverseStringFrequencies.rend(); ++i, ++it2)
    {
        for (size_t j = 0; j < it2->second.size(); ++j, ++i)
        {
            if (i < symbols.size())
                encodedStrings.insert(std::make_pair(it2->second[j], symbols[i]));
        }
    }

    output.WriteDynInt(encodedStrings.size());
    for(encodedStringsIt = encodedStrings.begin(); encodedStringsIt != encodedStrings.end(); ++encodedStringsIt)
    {
    	output.WriteUInt8(encodedStringsIt->second);
    	output.WriteString(encodedStringsIt->first);
    }
    
    in = std::istringstream((char *)input.Data());

    std::string str = in.str();

    std::ostringstream out;
    //int end = input.Data()[input.Size() - 1];

    while (in.good())
    {
        std::streamoff prev = in.tellg();
    	in >> temp;
        std::streamoff after = in.tellg();

    	if ((encodedStringsIt = encodedStrings.find(temp)) != encodedStrings.end())
    		output.WriteString(std::string(1,encodedStringsIt->second));
    	else
    		output.WriteString(temp); 

        if (after != -1)
            output.WriteDynInt(after - prev - temp.size());
        else
            output.WriteDynInt(input.Size() - prev - temp.size() - 1);
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

    std::stringstream str;

	std::string temp;
	while (hack->CanRead())
	{
        temp = hack->ReadString();
        size_t spaceCount = hack->ReadDynInt();

        for (size_t i = 0; i < spaceCount; ++i)
            str << ' ';

		if (temp.size() > 0 && (encodedSymbolsIt = encodedSymbols.find(temp.at(0))) != encodedSymbols.end())
			str << encodedSymbolsIt->second;
		else
			str << temp;
	};

    std::string r = str.str();
    output.WriteBuffer(r.c_str(), r.size());

	return true;
}
