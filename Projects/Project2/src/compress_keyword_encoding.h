#ifndef COMPRESS_KEYWORD_ENCODING_H_
#define COMPRESS_KEYWORD_ENCODING_H_

#include "compression_algorithm.h"

///! CompressKeywordEncoding class
/*!
    Keyword Encoding Compression algorithm implementation
*/
class CompressKeywordEncoding : public CompressionAlgorithm
{
    virtual bool CompressImpl(const ByteBuffer& input, ByteBuffer& output) override; ///< Compress implementation
    virtual bool DecompressImpl(const ByteBuffer& input, ByteBuffer& output) override; ///< Decompress implementation
    virtual const std::string& GetAlgorithm() const override { static std::string str = "key"; return str; }
};

#endif // COMPRESS_KEYWORD_ENCODING_H_
