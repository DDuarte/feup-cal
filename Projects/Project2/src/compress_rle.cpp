#include "compress_rle.h"
#include "data_input.h"

#include "bytebuffer.h"

#include <cctype>
#include <stdexcept>

#define MARKER_CHAR '*'
#define MIN_REPEATED 4

bool ValidateInput(const ByteBuffer& source)
{
    const char* in = (const char*)source.Data();

    for (size_t i = 0; i < source.Size() - 1; ++i)
        if (in[i] == MARKER_CHAR && isdigit(in[i + 1])) // example: *1A
            return false;

    return true;
}

bool CompressRLE::CompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
    if (!ValidateInput(input))
        return false;

    const char* in = (const char*)input.Data();

    for (size_t i = 0; i < input.Size(); ++i)
    {
        char c = in[i];

        if (i != input.Size() - 1) // can't be last char
        {
            size_t j = i + 1;
            int count = 1;
            while (in[j++] == c)
                count++;

            if (count >= MIN_REPEATED)
            {
                output.WriteUInt8(MARKER_CHAR); // especial marker

                char numStr[11];
                _itoa_s(count, numStr, 10);
                output.WriteBuffer(numStr, strlen(numStr)); // number of repeated 

                i += count - 1;
            }
        }

        output.WriteUInt8(c);
    }

    return true;
}

bool CompressRLE::DecompressImpl(const ByteBuffer& input, ByteBuffer& output)
{
    const char* in = (const char*)input.Data();

    for (size_t i = 0; i < input.Size(); ++i)
    {
        if (in[i] == MARKER_CHAR)
        {
            size_t digitCount = 0, j = i + 1;
            while (isdigit(in[j++]))
                digitCount++;

            int count = atoi(&in[i + 1]);
            for (int k = 0; k < count; ++k)
                output.WriteUInt8(in[i + 1 + digitCount]);

            i += digitCount + 1;
        }
        else
            output.WriteUInt8(in[i]);
    }

    return true;
}
