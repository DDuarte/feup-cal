#ifndef COMPRESS_HUFFMAN_ADAPTIVE_H_
#define COMPRESS_HUFFMAN_ADAPTIVE_H_

#include "compression_algorithm.h"

class CompressHuffmanAdaptive : public CompressionAlgorithm
{
    virtual bool CompressImpl(ByteBuffer& input, ByteBuffer& output) override;
    virtual bool DecompressImpl(ByteBuffer& input, ByteBuffer& output) override;
};

#endif // COMPRESS_HUFFMAN_ADAPTIVE_H_
