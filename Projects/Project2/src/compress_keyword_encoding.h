#ifndef COMPRESS_KEYWORD_ENCODING_H_
#define COMPRESS_KEYWORD_ENCODING_H_

#include "compression_algorithm.h"

class CompressKeywordEncoding : public CompressionAlgorithm
{
    virtual bool CompressImpl(ByteBuffer& source, ByteBuffer& output) override;
    virtual bool DecompressIml(ByteBuffer& input, ByteBuffer& result) override;
};

#endif // COMPRESS_KEYWORD_ENCODING_H_
