#ifndef COMPRESS_RLE_H_
#define COMPRESS_RLE_H_

#include "compression_algorithm.h"

///! CompressRLE class
/*!
    Run-length encoding (RLE) Compression algorithm implementation
*/
class CompressRLE : public CompressionAlgorithm
{
    virtual bool CompressImpl(const ByteBuffer& input, ByteBuffer& output) override; ///< Compress implementation
    virtual bool DecompressImpl(const ByteBuffer& input, ByteBuffer& output) override; ///< Decompress implementation
};

#endif // COMPRESS_RLE_H_
