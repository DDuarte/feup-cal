#ifndef COMPRESS_LZW_H_
#define COMPRESS_LZW_H_

#include "compression_algorithm.h"

///! CompressLZW class
/*!
    Lempel–Ziv–Welch (LZW) Compression algorithm implementation
*/
class CompressLZW : public CompressionAlgorithm
{
    virtual bool CompressImpl(const ByteBuffer& input, ByteBuffer& output) override; ///< Compress implementation
    virtual bool DecompressImpl(const ByteBuffer& input, ByteBuffer& output) override; ///< Decompress implementation
    virtual const std::string& GetAlgorithm() const override { static std::string str = "lzw"; return str; }
};

#endif // COMPRESS_LZW_H_
