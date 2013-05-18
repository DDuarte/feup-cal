#ifndef COMPRESS_KEYWORD_ENCODING_H_
#define COMPRESS_KEYWORD_ENCODING_H_

#include "compression_algorithm.h"

class CompressKeywordEncoding : public CompressionAlgorithm
{
    virtual bool CompressImpl(const ByteBuffer& input, ByteBuffer& output) override;
    virtual bool DecompressImpl(const ByteBuffer& input, ByteBuffer& output) override;
};

#endif // COMPRESS_KEYWORD_ENCODING_H_
