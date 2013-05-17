#include "compression_algorithm.h"


bool CompressionAlgorithm::Compress(ByteBuffer& input, ByteBuffer& output, Benchmark& benchmark)
{
    // init stats

    bool result = CompressImpl(input, output);

    // end stats

    return result;
}

bool CompressionAlgorithm::Decompress(ByteBuffer& input, ByteBuffer& output, Benchmark& benchmark)
{
    // init stats

    bool result = DecompressImpl(input, output);

    // end stats

    return result;
}
