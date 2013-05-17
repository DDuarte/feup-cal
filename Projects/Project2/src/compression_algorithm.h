#ifndef COMPRESSION_ALGORITHM_H_
#define COMPRESSION_ALGORITHM_H_

class Benchmark;
class ByteBuffer;

class CompressionAlgorithm
{
public:
    bool Compress(ByteBuffer& source, ByteBuffer& output, Benchmark& benchmark) { return false; }
    bool Decompress(ByteBuffer& input, ByteBuffer& result, Benchmark& benchmark) { return false; }

protected:
    virtual bool CompressImpl(ByteBuffer& source, ByteBuffer& output) = 0;
    virtual bool DecompressIml(ByteBuffer& input, ByteBuffer& result) = 0;
};

#endif // COMPRESSION_ALGORITHM_H_
