#ifndef COMPRESS_HUFFMAN_ADAPTIVE_H_
#define COMPRESS_HUFFMAN_ADAPTIVE_H_

#include "compression_algorithm.h"

///! CompressHuffmanAdaptive class
/*!
    Adaptive Huffman Compression algorithm implementation
*/
class CompressHuffmanAdaptive : public CompressionAlgorithm
{
    virtual bool CompressImpl(const ByteBuffer& input, ByteBuffer& output) override; ///< Compress implementation
    virtual bool DecompressImpl(const ByteBuffer& input, ByteBuffer& output) override; ///< Decompress implementation
};

#endif // COMPRESS_HUFFMAN_ADAPTIVE_H_
