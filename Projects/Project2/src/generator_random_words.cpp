#include "generator_random_words.h"

#include "file.h"
#include "bytebuffer.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>

GeneratorRandomWords::GeneratorRandomWords(const std::string& dictionaryFileName, int maxDictionaryWords, uint wordCount) : DataInput((int)wordCount)
{
    assert(!dictionaryFileName.empty());

    std::ifstream dictionary(dictionaryFileName);
    if (!dictionary.is_open())
    {
        std::ostringstream ss;
        ss << "Could not open file " << dictionaryFileName;
        throw DataInputException(ss.str());
    }

    std::vector<std::string> words;

    std::string word;
    while (dictionary.good())
    {
        std::getline(dictionary, word);
        words.push_back(word);
    }

    srand((unsigned int) time(NULL));

    while (_words.size() != maxDictionaryWords)
        _words.emplace_back(words[rand() % words.size()]);

    dictionary.close();
}

ByteBuffer GeneratorRandomWords::GenerateData() const 
{
    std::random_device generator;
    std::uniform_int_distribution<int> distribution(0, _words.size() - 1);

    std::stringstream ss;

    for (int i = 0; i < GetMaxSize(); ++i)
    {
        ss << _words[distribution(generator)];
        if (i != GetMaxSize() - 1)
            ss << ' ';
    }

    std::string str = ss.str();

    ByteBuffer bb((uint32) str.length());

    bb.WriteBuffer(ss.str().c_str(), (uint64) str.length());
    bb.WriteUInt8(0);

    return bb;
}
