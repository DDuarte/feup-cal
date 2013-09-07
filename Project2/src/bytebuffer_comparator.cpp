#include "bytebuffer_comparator.h"
#include "bytebuffer.h"

#include <cstring>

bool ByteBufferComparator::Equal(const ByteBuffer& b1, const ByteBuffer& b2)
{
    if (b1.Size() != b2.Size())
        return false;

    return std::memcmp(b1.Data(), b2.Data(), b1.Size()) == 0;
}

double ByteBufferComparator::CompareSizes(const ByteBuffer& b1, const ByteBuffer& b2)
{
    return b1.Size() / (double)b2.Size();
}
