#ifndef GENERATOR_RANDOM_CHARS_H_
#define GENERATOR_RANDOM_CHARS_H_

#include "data_input.h"
#include "utils.h"

#include <string>
#include <vector>

//! GeneratorRandomChars
/*!
    A DataInput implementation that generates Random characters
*/
class GeneratorRandomChars : public DataInput
{
public:
    GeneratorRandomChars(uint charCount); ///< Constructor

    virtual ByteBuffer GenerateData() const override; ///< Returns a ByteBuffer instance with random characters
};

#endif // GENERATOR_RANDOM_CHARS_H_
