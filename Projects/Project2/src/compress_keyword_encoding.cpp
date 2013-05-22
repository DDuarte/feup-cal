#include "compress_keyword_encoding.h"
#include "bytebuffer.h"

#include <stdexcept>
#include <sstream>
#include <map>
#include <unordered_map>

std::string  initializeSymbols()
{
	std::string s;
	for(int i = 32; i < 127; ++i)
		s += i;

	return s;
}

static std::string symbols = initializeSymbols();

bool CompressKeywordEncoding::CompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
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

	std::map<unsigned int, std::vector<std::string>> foda;
	std::map<unsigned int, std::vector<std::string>>::iterator it1;

	for (const auto& elem: stringFrequencies)
	{
		it1 = foda.find(elem.second);
		if (it1 != foda.end())
		{
			it1->second.push_back(elem.first);
		}
		else
		{
			it1 = foda.insert(it1, std::make_pair(elem.second, std::vector<std::string>()));
			it1->second.push_back(elem.first);
		}
	}

	std::unordered_map<std::string, char> encodedStrings;
	std::unordered_map<std::string, char>::iterator encodedStringsIt;
	it1 = foda.begin();
	for(std::string::iterator it_s = symbols.begin(); it_s != symbols.end() && it1 != foda.end(); ++it_s, ++it1)
		for(size_t i=0; i < it1->second.size(); ++i, ++it_s)
		{
			encodedStrings.insert(std::make_pair(it1->second[i], *it_s));
			if(it_s == symbols.end())
				break;
		}


		output.WriteDynInt(encodedStrings.size());
		for(encodedStringsIt = encodedStrings.begin(); encodedStringsIt != encodedStrings.end(); ++encodedStringsIt)
		{
			output.WriteUInt8(encodedStringsIt->second);
			output.WriteString(encodedStringsIt->first);
		}

		in = std::istringstream((char *)input.Data());
		std::ostringstream out;
		while(in.good())
		{
			in >> temp;
			if((encodedStringsIt = encodedStrings.find(temp)) != encodedStrings.end())
				output.WriteString(std::string(1,encodedStringsIt->second));//out << encodedStringsIt->second;
			else
				output.WriteString(temp); //out << temp;
		}

		/*temp = out.str();
		output.WriteBuffer(temp.c_str(), temp.size());*/
		//output.WriteUInt8(0);
		return true;
}

bool CompressKeywordEncoding::DecompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
	ByteBuffer* hack = const_cast<ByteBuffer*>(&input);
	
	std::unordered_map<char, std::string> encodedSymbols;
	std::unordered_map<char, std::string>::iterator encodedSymbolsIt;
	
	int numTableEntries;
	numTableEntries = hack->ReadDynInt();

	char symbol; 
	std::string expression;
	for(; numTableEntries > 0; --numTableEntries)
	{
		symbol = (char)hack->ReadUInt8();
		expression = hack->ReadString();
		encodedSymbols.insert(std::make_pair(symbol, expression));
	}

	std::string temp;
	do
	{
		temp = hack->ReadString();

		if(temp.size() > 0 && (encodedSymbolsIt = encodedSymbols.find(temp.at(0))) != encodedSymbols.end())
			output.WriteString(encodedSymbolsIt->second);
		else
			output.WriteString(temp);
	} while(hack->GetReadPos() != hack->GetWritePos());

	return true;
}
