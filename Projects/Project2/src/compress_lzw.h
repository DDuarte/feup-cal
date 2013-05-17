#ifndef COMPRESS_LZW_H_
#define COMPRESS_LZW_H_

#include "compression_algorithm.h"

class CompressLZW : public CompressionAlgorithm
{
    virtual bool CompressImpl(ByteBuffer& input, ByteBuffer& output) override;
    virtual bool DecompressImpl(ByteBuffer& input, ByteBuffer& output) override;
};

#endif // COMPRESS_LZW_H_
