#include "compression_algorithm.h"

#include <ctime>


bool CompressionAlgorithm::Compress(ByteBuffer& input, ByteBuffer& output, Benchmark& benchmark)
{
    // init stats

    clock_t t;

    t = clock();

    // compute

    bool result = CompressImpl(input, output);

    // end stats

    double secs = t / (double) CLOCKS_PER_SEC;

    return result;
}

bool CompressionAlgorithm::Decompress(ByteBuffer& input, ByteBuffer& output, Benchmark& benchmark)
{
    // init stats

    clock_t t;

    t = clock();

    // compute

    bool result = DecompressImpl(input, output);

    // end stats

    double secs = t / (double) CLOCKS_PER_SEC;

    return result;
}
