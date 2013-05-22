#include "compress_rle.h"
#include "data_input.h"

#include "bytebuffer.h"

#include <cctype>
#include <stdexcept>

#define MARKER_CHAR 21 // NAK
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
                output.WriteDynInt(count);

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
    ByteBuffer* hack = const_cast<ByteBuffer*>(&input);

    for (size_t i = 0; i < input.Size(); ++i)
    {
        if (in[i] == MARKER_CHAR && (i < input.Size() - 1))
        {
            hack->SetReadPos(i + 1);
            int count = hack->ReadDynInt();
            for (int k = 0; k < count; ++k)
                output.WriteUInt8(in[i + 1 + hack->GetReadPos() - (i + 1)]);

            i += hack->GetReadPos() - (i + 1) + 1;
        }
        else
            output.WriteUInt8(in[i]);
    }

    return true;
}
