#ifndef COMPRESS_RLE_H_
#define COMPRESS_RLE_H_

#include "compression_algorithm.h"

class CompressRLE : public CompressionAlgorithm
{
    virtual bool CompressImpl(ByteBuffer& source, ByteBuffer& output) override;
    virtual bool DecompressIml(ByteBuffer& input, ByteBuffer& result) override;
};

#endif // COMPRESS_RLE_H_
