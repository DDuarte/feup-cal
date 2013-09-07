#ifndef GENERATOR_RANDOM_WORDS_H_
#define GENERATOR_RANDOM_WORDS_H_

#include "data_input.h"
#include "utils.h"

#include <string>
#include <vector>

//! GeneratorRandomWords class
/*!
    A DataInput implementation that returns random words from a given dictionary file
*/
class GeneratorRandomWords : public DataInput
{
public:
    GeneratorRandomWords(const std::string& dictionaryFileName, int maxDictionaryWords, uint wordCount); ///< Constructor

    virtual ByteBuffer GenerateData() const override; ///< Returns a ByteBuffer instance with random words from the dictionary

private:
    std::vector<std::string> _words;
};

#endif // GENERATOR_RANDOM_WORDS_H_
