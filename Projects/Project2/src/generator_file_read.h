#ifndef GENERATOR_FILE_READ_H_
#define GENERATOR_FILE_READ_H_

#include "data_input.h"
#include "bytebuffer.h"

#include <string>

class GeneratorFileRead : public DataInput
{
public:
    GeneratorFileRead(const std::string& fileName, int maxBytes = -1);

    virtual ByteBuffer GenerateData() const override { return _bb; }

private:
    std::string _fileName;
    ByteBuffer _bb;
};

#endif // GENERATOR_FILE_READ_H_
