#ifndef COMPRESSION_ALGORITHM_H_
#define COMPRESSION_ALGORITHM_H_

class Benchmark;
class ByteBuffer;

class CompressionAlgorithm
{
public:
    /* virtual */ bool Compress(ByteBuffer& input, ByteBuffer& output, Benchmark& benchmark) /* final */;
    /* virtual */ bool Decompress(ByteBuffer& input, ByteBuffer& output, Benchmark& benchmark) /* final */;

protected:
    virtual bool CompressImpl(ByteBuffer& input, ByteBuffer& output) = 0;
    virtual bool DecompressImpl(ByteBuffer& input, ByteBuffer& output) = 0;
};

#endif // COMPRESSION_ALGORITHM_H_
