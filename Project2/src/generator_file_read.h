#ifndef GENERATOR_FILE_READ_H_
#define GENERATOR_FILE_READ_H_

#include "data_input.h"
#include "bytebuffer.h"

#include <string>

//! GeneratorFileRead class
/*!
    A DataInput implementation that reads the data from a given file
*/
class GeneratorFileRead : public DataInput
{
public:
    GeneratorFileRead(const std::string& fileName, int maxBytes = -1); ///< Constructor

    virtual ByteBuffer GenerateData() const override { return _bb; } ///< Data Generator

private:
    std::string _fileName;
    ByteBuffer _bb;
};

#endif // GENERATOR_FILE_READ_H_
