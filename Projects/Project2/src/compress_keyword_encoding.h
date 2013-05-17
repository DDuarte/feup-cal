#ifndef COMPRESS_KEYWORD_ENCODING_H_
#define COMPRESS_KEYWORD_ENCODING_H_

#include "compression_algorithm.h"

class CompressKeywordEncoding : public CompressionAlgorithm
{
    virtual bool CompressImpl(ByteBuffer& input, ByteBuffer& output) override;
    virtual bool DecompressImpl(ByteBuffer& input, ByteBuffer& output) override;
};

#endif // COMPRESS_KEYWORD_ENCODING_H_
