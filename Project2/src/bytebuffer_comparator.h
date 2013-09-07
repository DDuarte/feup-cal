#ifndef BYTEBUFFER_COMPARATOR_H_
#define BYTEBUFFER_COMPARATOR_H_

class ByteBuffer;

//! ByteBufferComparator class
/*!
    Compares ByteBuffers
*/
class ByteBufferComparator
{
public:
    static bool Equal(const ByteBuffer& b1, const ByteBuffer& b2); ///< Indicates whether or not the given ByteBuffers are equal
    static double CompareSizes(const ByteBuffer& b1, const ByteBuffer& b2); ///< Returns the difference between the sizes of the the given ByteBuffers
};

#endif // BYTEBUFFER_COMPARATOR_H_
