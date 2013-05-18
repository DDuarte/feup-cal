#ifndef COMPRESS_RLE_H_
#define COMPRESS_RLE_H_

#include "compression_algorithm.h"

class CompressRLE : public CompressionAlgorithm
{
    virtual bool CompressImpl(const ByteBuffer& input, ByteBuffer& output) override;
    virtual bool DecompressImpl(const ByteBuffer& input, ByteBuffer& output) override;
};

#endif // COMPRESS_RLE_H_
