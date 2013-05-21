#ifndef GENERATOR_RANDOM_CHARS_H_
#define GENERATOR_RANDOM_CHARS_H_

#include "data_input.h"
#include "utils.h"

#include <string>
#include <vector>

class GeneratorRandomChars : public DataInput
{
public:
    GeneratorRandomChars(uint charCount);

    virtual ByteBuffer GenerateData() const override;
};

#endif // GENERATOR_RANDOM_CHARS_H_
