#ifndef COMPRESS_LZW_H_
#define COMPRESS_LZW_H_

#include "compression_algorithm.h"

class CompressLZW : public CompressionAlgorithm
{
    virtual bool CompressImpl(ByteBuffer& source, ByteBuffer& output) override;
    virtual bool DecompressIml(ByteBuffer& input, ByteBuffer& result) override;
};

#endif // COMPRESS_LZW_H_
