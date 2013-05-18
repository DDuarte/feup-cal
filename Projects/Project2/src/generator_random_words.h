#ifndef GENERATOR_RANDOM_WORDS_H_
#define GENERATOR_RANDOM_WORDS_H_

#include "data_input.h"
#include "utils.h"

#include <string>
#include <vector>

class GeneatorRandomWords : public DataInput
{
public:
    GeneatorRandomWords(const std::string& dictionaryFileName, int maxDictionaryWords, uint wordCount);

    virtual ByteBuffer GenerateData() const override;

private:
    std::vector<std::string> _words;
};

#endif // GENERATOR_RANDOM_WORDS_H_
