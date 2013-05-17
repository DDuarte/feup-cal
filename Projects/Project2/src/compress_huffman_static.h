#ifndef COMPRESS_HUFFMAN_STATIC_H_
#define COMPRESS_HUFFMAN_STATIC_H_

#include "compression_algorithm.h"

class CompressHuffmanStatic : public CompressionAlgorithm
{
    virtual bool CompressImpl(ByteBuffer& source, ByteBuffer& output) override;
    virtual bool DecompressIml(ByteBuffer& input, ByteBuffer& result) override;
};

#endif // COMPRESS_HUFFMAN_STATIC_H_
