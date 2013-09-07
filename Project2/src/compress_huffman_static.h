#ifndef COMPRESS_HUFFMAN_STATIC_H_
#define COMPRESS_HUFFMAN_STATIC_H_

#include "compression_algorithm.h"

///! CompressHuffmanStatic class
/*!
    Static Huffman Compression algorithm implementation
*/
class CompressHuffmanStatic : public CompressionAlgorithm
{
    virtual bool CompressImpl(const ByteBuffer& input, ByteBuffer& output) override; ///< Compress implementation
    virtual bool DecompressImpl(const ByteBuffer& input, ByteBuffer& output) override; ///< Decompress implementation
    virtual const std::string& GetAlgorithm() const override { static std::string str = "huffsta"; return str; }
};

#endif // COMPRESS_HUFFMAN_STATIC_H_
