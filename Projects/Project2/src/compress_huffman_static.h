#ifndef COMPRESS_HUFFMAN_STATIC_H_
#define COMPRESS_HUFFMAN_STATIC_H_

#include "compression_algorithm.h"

class CompressHuffmanStatic : public CompressionAlgorithm
{
    virtual bool CompressImpl(const ByteBuffer& input, ByteBuffer& output) override;
    virtual bool DecompressImpl(const ByteBuffer& input, ByteBuffer& output) override;
};

#endif // COMPRESS_HUFFMAN_STATIC_H_
