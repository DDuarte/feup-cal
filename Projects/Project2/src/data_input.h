#ifndef DATA_INPUT_H_
#define DATA_INPUT_H_

#include "utils.h"

#include <string>
#include <stdexcept>

class ByteBuffer;

/// DataInputException
class DataInputException : public std::runtime_error
{
public:
    DataInputException(const std::string& what) : runtime_error(what) { }
};

//! Abstract DataInput class
/*!
    Represents an object the can generate data
*/
class DataInput
{
public:
    DataInput(int sizeLimit = -1) : _noLimit(sizeLimit == -1), _size(sizeLimit) { } ///< Constructor
    virtual ByteBuffer GenerateData() const = 0; ///< Returns a ByteBuffer instance with the generated data
    int GetMaxSize() const { return _size; } ///< Returns the maximum size of the data
    bool HasMaxLimit() const { return !_noLimit; } ///< Indicates if there is a maximum limit to the size of the data

private:
    int _size;
    bool _noLimit;
};

#endif // DATA_INPUT_H_
