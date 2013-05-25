#ifndef COMPRESSION_ALGORITHM_H_
#define COMPRESSION_ALGORITHM_H_

#include <string>

class BenchmarkData;
class ByteBuffer;

///! Abstract CompressionAlgorithm class
/*!
    Represents a Compression Algorithm
*/
class CompressionAlgorithm
{
public:
    /* virtual */ bool Compress(const ByteBuffer& input, ByteBuffer& output, BenchmarkData& benchmark) /* final */; ///< Compress Function that calls the CompressImpl method
    /* virtual */ bool Decompress(const ByteBuffer& input, ByteBuffer& output, BenchmarkData& benchmark) /* final */; ///< Decompress Function that calls the DecompressImpl method

protected:
    virtual bool CompressImpl(const ByteBuffer& input, ByteBuffer& output) = 0; ///< Implementation of the compress function (to be overridden by subclasses)
    virtual bool DecompressImpl(const ByteBuffer& input, ByteBuffer& output) = 0; ///< Implementation of the decompress function (to be overridden by subclasses)
    virtual const std::string& GetAlgorithm() const = 0; ///< Identification of the algorithm (to be overridden by subclasses)
};

#endif // COMPRESSION_ALGORITHM_H_
