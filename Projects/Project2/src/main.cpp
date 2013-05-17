#include <iostream>
#include <map>
#include <string>

#include "compression_algorithm.h"
#include "compress_rle.h"
#include "compress_lzw.h"
#include "compress_huffman_static.h"
#include "compress_huffman_adaptive.h"
#include "compress_keyword_encoding.h"


int main(int argc, char** argv)
{
    typedef std::map<std::string, CompressionAlgorithm*> CompressMap;

    CompressMap algorithms;
    algorithms["rle"] = new CompressRLE();
    algorithms["lzw"] = new CompressLZW();
    algorithms["huff_adaptive"] = new CompressHuffmanAdaptive();
    algorithms["huff_static"] = new CompressHuffmanStatic();
    algorithms["keyword"] = new CompressKeywordEncoding();

    std::cout << "Available algorithms:" << std::endl;
    for (CompressMap::value_type c : algorithms)
        std::cout << "\t- " << c.first << std::endl;

    for (CompressMap::value_type c : algorithms)
        delete c.second;

    system("PAUSE");
}
