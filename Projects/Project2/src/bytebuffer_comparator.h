#ifndef BYTEBUFFER_COMPARATOR_H_
#define BYTEBUFFER_COMPARATOR_H_

class ByteBuffer;

class ByteBufferComparator
{
public:
    static bool Equal(const ByteBuffer& b1, const ByteBuffer& b2);
    static double CompareSizes(const ByteBuffer& b1, const ByteBuffer& b2);
};

#endif // BYTEBUFFER_COMPARATOR_H_
