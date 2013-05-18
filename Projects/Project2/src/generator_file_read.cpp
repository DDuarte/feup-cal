#include "generator_file_read.h"

#include "file.h"

#include <cassert>
#include <sstream>

GeneratorFileRead::GeneratorFileRead(const std::string& fileName, int maxBytes /*= -1*/) : DataInput(maxBytes), _fileName(fileName), _bb(100)
{
    assert(!_fileName.empty());

    char* buffer;
    size_t size;

    if (!File::Load(_fileName.c_str(), buffer, size))
    {
        std::ostringstream ss;
        ss << "Could not open file " << _fileName;
        throw DataInputException(ss.str());
    }

    //ByteBuffer bb(HasMaxLimit() ? GetMaxSize() : size);
    _bb.WriteBuffer(buffer, HasMaxLimit() ? GetMaxSize() : size);
    //_bb = bb;
}
