#ifndef DATA_INPUT_H_
#define DATA_INPUT_H_

#include "utils.h"

#include <string>
#include <stdexcept>

class ByteBuffer;

class DataInputException : public std::runtime_error
{
public:
    DataInputException(const std::string& what) : runtime_error(what) { }
};

class DataInput
{
public:
    DataInput(int sizeLimit = -1) : _noLimit(sizeLimit == -1), _size(sizeLimit) { }
    virtual ByteBuffer GenerateData() const = 0;
    int GetMaxSize() const { return _size; }
    bool HasMaxLimit() const { return !_noLimit; }

private:
    int _size;
    bool _noLimit;
};

#endif // DATA_INPUT_H_
