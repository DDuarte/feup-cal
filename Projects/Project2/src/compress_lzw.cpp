#include "compress_lzw.h"
#include "bytebuffer.h"

#include <stdexcept>
#include <map>
#include <iostream>
#include <sstream>

#define DEFAULT_DICTIONARY_SIZE 256
#define MARKER_CHAR 21 // NAK

bool CompressLZW::CompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
	int dictSize = DEFAULT_DICTIONARY_SIZE;
	std::map<std::string, int> dictionary;
	for (int i = 0; i < DEFAULT_DICTIONARY_SIZE; i++)
		dictionary[std::string(1, i)] = i;

	const char* in = (const char*)input.Data();
	std::cout << "Data: " << in << std::endl;

	std::string w;
	char c;
	for (size_t i = 0; i < input.Size(); ++i)
	{
		c = input[i];
		std::string wc = w + c;
		if(dictionary.count(wc))
			w = wc;
		else
		{
			int mappedValue = dictionary[w];
			output.WriteUInt32(mappedValue);
			dictionary[wc] = dictSize++;
			w = std::string(1, c);
		}
	}

	if(!w.empty())
		output.WriteUInt32(dictionary[w]);
	/*
	size_t pos = output.GetWritePos();
	output.SetWritePos(0);
	output.WriteUInt32(dictionary.size());
	output.SetWritePos(pos);
	for (std::map<std::string, int>::value_type & p : dictionary)
	{
	output.WriteDynInt(p.second);
	output.WriteString(p.first); 
	}
	*/
	return true;
}

bool CompressLZW::DecompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
	int dictSize = DEFAULT_DICTIONARY_SIZE;
	std::map<int,std::string> dictionary;
	for (int i = 0; i < DEFAULT_DICTIONARY_SIZE; i++)
		dictionary[i] = std::string(1, i);

	ByteBuffer* hack = const_cast<ByteBuffer*>(&input);
	std::stringstream out;

	//std::string w(1, hack->ReadUInt32());
	std::string w;
	//output.WriteString(w);
	std::string entry;
	int i = 0;
	do{
		int k = hack->ReadUInt32();
		if(i == 0)
		{
			w = std::string(1, k);
			//output.WriteString(w);
			out << w;
			++i;
		}
		if (dictionary.count(k))
			entry = dictionary[k];
		else if (k == dictSize)
			entry = w + w[0];
		else
			throw std::exception("The input file hasn't been compressed properly");
		//result += entry;
		//output.WriteString(entry);
		out << entry;

		// Add w+entry[0] to the dictionary.
		dictionary[dictSize++] = w + entry[0];

		w = entry;
	} while(hack->CanRead());

	std::string s = out.str();
	output.WriteBuffer(s.c_str(), s.size());
	output.WriteInt8(0);
	return true;
}
