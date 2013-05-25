#include "compression_algorithm.h"
#include "benchmark.h"
#include "bytebuffer.h"

#include <ctime>

bool CompressionAlgorithm::Compress(const ByteBuffer& input, ByteBuffer& output, Benchmark& benchmark)
{
    // init stats

    benchmark.SetCompression(true);
    benchmark.SetAlgorithm(GetAlgorithm());
    benchmark.SetOriginalSize(input.Size());
    benchmark.SetStartTime(clock() / (double) CLOCKS_PER_SEC);

    // compute

    bool result = CompressImpl(input, output);


    // end stats

    benchmark.SetEndTime(clock() / (double) CLOCKS_PER_SEC);
    benchmark.SetResultSize(output.Size());

    return result;
}

bool CompressionAlgorithm::Decompress(const ByteBuffer& input, ByteBuffer& output, Benchmark& benchmark)
{
    // init stats

    benchmark.SetCompression(false);
    benchmark.SetAlgorithm(GetAlgorithm());
    benchmark.SetOriginalSize(input.Size());
    benchmark.SetStartTime(clock() / (double) CLOCKS_PER_SEC);

    // compute

    bool result = DecompressImpl(input, output);

    // end stats

    benchmark.SetEndTime(clock() / (double) CLOCKS_PER_SEC);
    benchmark.SetResultSize(output.Size());

    return result;
}
