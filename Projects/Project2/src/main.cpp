#include <boost/program_options.hpp>
#include <vector>
#include <string>
#include <stdlib.h>
#include <map>
#include <stdexcept>
#include <algorithm>

#include "file.h"

#include "data_input.h"
#include "generator_file_read.h"
#include "generator_random_chars.h"
#include "generator_random_words.h"

#include "compression_algorithm.h"
#include "compress_rle.h"
#include "compress_lzw.h"
#include "compress_keyword_encoding.h"
#include "compress_huffman_static.h"
#include "compress_huffman_adaptive.h"

#include "bytebuffer.h"
#include "bytebuffer_comparator.h"
#include "benchmark_data.h"

// damn you, msvc
#undef max
#undef min

static std::vector<std::string> selectedAlgorithms;
static std::vector<std::string> files;
static std::multimap<std::string, BenchmarkData> benchmarks;
static std::vector<BenchmarkData> benchmarks_gen;
static bool compress = false;
static bool decompress = false;
static bool plot = false;
static bool stats = false;

int main(int argc, char** argv)
{
    namespace po = boost::program_options;

    po::options_description genericOptions("Generic options");
    genericOptions.add_options()
        ("version,v", "print version string")
        ("help,h",    "produce help message");

    po::options_description algorithmOptions("Available algorithms");
    algorithmOptions.add_options()
        ("lzw",     "Run Lempel-Ziv-Welch")
        ("rle",     "Run Run-length encoding")
        ("key",     "Run Keyword encoding")
        ("huffdyn", "Run dynamic Huffman coding")
        ("huffsta", "Run static Huffman coding")
        ("all",     "Run all the above, except RLE because it's SLOW.");

    po::options_description generatorOptions("Generators");
    generatorOptions.add_options()
        ("generator", po::value<std::string>(), "Data generator, can be 'words' or 'chars'.")
        ("gen-min", po::value<int>()->default_value(1), "Minimum number of words or chars.")
        ("gen-max", po::value<int>()->default_value(100), "Maximum number of words or chars.")
        ("gen-count", po::value<int>()->default_value(10), "Number of files to create.");

    po::options_description configOptions("Configuration");
    configOptions.add_options()
        ("compress,c", "Compress file")
        ("decompress,d", "Decompress file")
        ("plot,p", "Create charts and plots comparing the selected algorithms")
        ("stats,s", "Write simple statistics about compression and/or decompression");

    po::options_description hiddenOptions("Hidden options");
    hiddenOptions.add_options()
        ("input-file", po::value<std::vector<std::string>>(), "Files to compress and/or decompress");

    po::positional_options_description p;
    p.add("input-file", -1);

    po::options_description options;
    options.add(genericOptions).add(algorithmOptions).add(generatorOptions).add(configOptions).add(hiddenOptions);

    po::options_description visibleOptions;
    visibleOptions.add(genericOptions).add(generatorOptions).add(algorithmOptions).add(configOptions);

    po::variables_map vm;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(options).positional(p).run(), vm);
        po::notify(vm);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    decompress = vm.count("decompress") != 0;
    compress = vm.count("compress") != 0;
    stats = vm.count("stats") != 0;
    plot = vm.count("plot") != 0;

    if (vm.count("help"))
    {
        std::cout << visibleOptions << std::endl;
        return EXIT_SUCCESS;
    }

    if (vm.count("version"))
    {
        std::cout << "calzip version 1.0.0.who_the_hell_cares" << std::endl;
        return EXIT_SUCCESS;
    }

    if (vm.count("all"))
    {
        selectedAlgorithms.push_back("lzw");
        selectedAlgorithms.push_back("rle");
        // selectedAlgorithms.push_back("key"); too slow, disabled by default
        selectedAlgorithms.push_back("huffdyn");
        selectedAlgorithms.push_back("huffsta");
    }
    else
    {
        if (vm.count("lzw")) selectedAlgorithms.push_back("lzw");
        if (vm.count("rle")) selectedAlgorithms.push_back("rle");
        if (vm.count("key")) selectedAlgorithms.push_back("key");
        if (vm.count("huffdyn")) selectedAlgorithms.push_back("huffdyn");
        if (vm.count("huffsta")) selectedAlgorithms.push_back("huffsta");

        if (selectedAlgorithms.empty())
            selectedAlgorithms.push_back("lzw"); // default if none set
    }

    if (vm.count("input-file") == 1)
    {
        for (auto& f : vm["input-file"].as<std::vector<std::string>>())
        {
            if (File::Exists(f.c_str()))
                files.push_back(f);
            else
                std::cerr << "Specified file " << f << " does not exists." << std::endl;
        }
    }

    if (files.empty() && !vm.count("generator"))
    {
        std::cerr << "No input files to process or data to generate." << std::endl;
        std::cout << visibleOptions << std::endl;
        return EXIT_FAILURE;
    }

    if (!files.empty() && vm.count("generator"))
    {
        std::cerr << "Can't process files and generate data at the same time. Sorry." << std::endl;
        return EXIT_FAILURE;
    }

    typedef std::map<std::string, CompressionAlgorithm*> CompressMap;

    CompressMap algorithms;
    algorithms["rle"] = new CompressRLE();
    algorithms["lzw"] = new CompressLZW();
    algorithms["huffdyn"] = new CompressHuffmanAdaptive();
    algorithms["huffsta"] = new CompressHuffmanStatic();
    algorithms["key"] = new CompressKeywordEncoding();

    for (auto& f : files)
    {
        DataInput* di = nullptr;
        try
        {
            di = new GeneratorFileRead(f);
            ByteBuffer in = di->GenerateData();

            char drive[_MAX_DRIVE];
            char dir[_MAX_DIR];
            char fname[_MAX_FNAME];
            char ext[_MAX_EXT];
            _splitpath_s(f.c_str(), drive, dir, fname, ext);

            for (auto& alg : selectedAlgorithms)
            {
                in.SetReadPos(0); // reset

                if (compress || (strcmp(ext, ".calzip") != 0))
                {
                    ByteBuffer out(in.Size() / 2);
                    BenchmarkData b;
                    if (!algorithms[alg]->Compress(in, out, b))
                    {
                        std::cerr << "Could not compress file " << f << " using " << alg << "." << std::endl;
                        continue;
                    }

                    benchmarks.insert(std::make_pair(f, b));

                    std::string filename = drive + std::string(dir) + fname + "_" + alg + ext + ".calzip";
                    if (!File::Save(filename.c_str(), (const char*)out.Data(), out.Size()))
                    {
                        std::cerr << "Error while trying to save file " << filename << " (" << f << ")." << std::endl;
                        continue;
                    }

                    if (decompress)
                    {
                        ByteBuffer out2(in.Size());
                        BenchmarkData b;
                        if (!algorithms[alg]->Decompress(out, out2, b))
                        {
                            std::cerr << "Could not decompress file " << f << " using " << alg << "." << std::endl;
                            continue;
                        }

                        benchmarks.insert(std::make_pair(f, b));

                        std::string filename = drive + std::string(dir) + fname + "_" + alg + ext;
                        if (!File::Save(filename.c_str(), (const char*)out2.Data(), out2.Size()))
                        {
                            std::cerr << "Error while trying to save file " << filename << " (" << f << ")." << std::endl;
                            continue;
                        }
                    }
                }
            }

            if ((decompress && !compress) || (strcmp(ext, ".calzip") == 0))
            {
                std::string alg;
                if (contains(std::string(fname), "_rle")) alg = "rle";
                else if (contains(std::string(fname), "_lzw")) alg = "lzw";
                else if (contains(std::string(fname), "_huffdyn")) alg = "huffdyn";
                else if (contains(std::string(fname), "_huffsta")) alg = "huffsta";
                else if (contains(std::string(fname), "_key")) alg = "key";
                else
                {
                    std::cerr << "Could not find algorithm to decompress from filename " << f << "." << std::endl;
                    continue;
                }

                ByteBuffer out(in.Size() / 2);
                BenchmarkData b;
                if (!algorithms[alg]->Decompress(in, out, b))
                {
                    std::cerr << "Could not decompress file " << f << " using " << alg << "." << std::endl;
                    continue;
                }

                benchmarks.insert(std::make_pair(f, b));

                std::string filename = drive + std::string(dir) + fname;
                if (!File::Save(filename.c_str(), (const char*)out.Data(), out.Size()))
                {
                    std::cerr << "Error while trying to save file " << filename << " (" << f << ")." << std::endl;
                    continue;
                }
            }
        }
        catch (std::exception& ex)
        {
            std::cerr << "Exception \"" << ex.what() << "\" occurred while processing file " << f << "." << std::endl;
        }

        if (di)
        {
            delete di;
            di = nullptr;
        }
    }

    if (vm.count("generator"))
    {
        int min = vm["gen-min"].as<int>();
        int max = vm["gen-max"].as<int>();
        int count = vm["gen-count"].as<int>();
        std::string generator = vm["generator"].as<std::string>();

        if (min < 1 || max < 1 || count < 1)
        {
            std::cerr << "Generator's min, max and count need to be higher than 0." << std::endl;
            return EXIT_FAILURE;
        }

        if (min > max)
        {
            std::cerr << "Generator's min needs to be lower than max." << std::endl;
            return EXIT_FAILURE;
        }

        if (selectedAlgorithms.size() > 1)
        {
            std::cerr << "Can only generate data for one algorithm (using " << selectedAlgorithms.size() << " atm)." << std::endl;
            return EXIT_FAILURE;
        }

        if (generator != "words" && generator != "chars")
        {
            std::cerr << "Selected generator (" << generator << ") not recognized." << std::endl;
            return EXIT_FAILURE;
        }

        int sizeInc = (max - min) / (count - 1);
        for (int i = 0; i < count; ++i)
        {
            DataInput* di = nullptr;
            if (generator == "words")
                di = new GeneratorRandomWords("wordsEn.txt", 1000, std::max(min, i * sizeInc));
            else if (generator == "chars")
                di = new GeneratorRandomChars(std::max(min, i * sizeInc));

            ByteBuffer in = di->GenerateData();

            ByteBuffer out(in.Size() / 2);
            BenchmarkData b;
            if (!algorithms[*selectedAlgorithms.begin()]->Compress(in, out, b))
            {
                std::cerr << "Could not compress data of generator '" << generator << "' with size " << i * sizeInc << "." << std::endl;
                continue;
            }

            benchmarks_gen.push_back(b);

            ByteBuffer out2(in.Size());
            if (!algorithms[*selectedAlgorithms.begin()]->Decompress(out, out2, b))
            {
                std::cerr << "Could not decompress data of generator '" << generator << "' with size " << i * sizeInc << "." << std::endl;
                continue;
            }

            benchmarks_gen.push_back(b);

            delete di;
        }
    }

    for (CompressMap::value_type c : algorithms)
        delete c.second;

    if (stats)
    {
        for (auto& p : benchmarks)
        {
            std::string action = p.second.IsCompression() ? "Compression" : "Decompression";

            std::cout << "File: " << p.first << std::endl;
            std::cout << "Algorithm: " << p.second.GetAlgorithm() << std::endl;
            std::cout << "\t" << action << " time: " << p.second.GetExecutedTime() << " ms" << std::endl;
            std::cout << "\t" << action << " rate: " << p.second.GetCompressionRate() << "%" << std::endl;
            std::cout << "\tOriginal size: " << p.second.GetOriginalSize() << " bytes" << std::endl;
            std::cout << "\tResult size: " << p.second.GetResultSize() << " bytes" << std::endl;
        }

        for (auto& b : benchmarks_gen)
        {
            std::string action = b.IsCompression() ? "Compression" : "Decompression";

            std::cout << "Algorithm: " << b.GetAlgorithm() << std::endl;
            std::cout << "\t" << action << " time: " << b.GetExecutedTime() << " ms" << std::endl;
            std::cout << "\t" << action << " rate: " << b.GetCompressionRate() << "%" << std::endl;
            std::cout << "\tOriginal size: " << b.GetOriginalSize() << " bytes" << std::endl;
            std::cout << "\tResult size: " << b.GetResultSize() << " bytes" << std::endl;
        }
    }

    if (plot)
    {
        size_t compressionCount = std::count_if(benchmarks.begin(), benchmarks.end(), [](std::multimap<std::string, BenchmarkData>::value_type& p) { return p.second.IsCompression(); });
        size_t decompressionCount = benchmarks.size() - compressionCount;

        if (compressionCount == decompressionCount && vm.count("input-file") == 1)
        {
            std::string title1 = "Comparison of Compression and Decompression Times (lower is better)";
            std::string columns1 = "data.addColumn('string', 'Algorithm');\n"
                                   "data.addColumn('number', 'Compression');\n"
                                   "data.addColumn('number', 'Decompression');\n";

            std::string data1;

            int i = 0;
            for (std::multimap<std::string, BenchmarkData>::iterator itr = benchmarks.begin(); itr != benchmarks.end(); ++itr, ++i)
            {
                if (!itr->second.IsCompression())
                    continue;

                std::string alg = itr->second.GetAlgorithm();
                std::string compressionTime = std::to_string(itr->second.GetExecutedTime());
                std::string decompressionTime = std::to_string((++itr)->second.GetExecutedTime());
                i++;

                data1 += "['" + alg + "', " + compressionTime + ", " + decompressionTime + ']';
                if (i != benchmarks.size() - 1)
                    data1 += ",\n";
                else
                    data1 += '\n';
            }

            std::string title2 = "Comparison of Compression Rates (higher is better)";
            std::string columns2 = "data.addColumn('string', 'Algorithm');\n"
                                   "data.addColumn('number', 'Rate');\n";

            std::string data2;

            i = 0;
            for (auto& p : benchmarks)
            {
                if (!p.second.IsCompression()) continue;
                data2 += "['" + p.second.GetAlgorithm() + "', " + std::to_string(p.second.GetCompressionRate()) + ']';
                if (++i != compressionCount)
                    data2 += ",\n";
                else
                    data2 += '\n';
            }

            // Warning: bad code hereafter

            size_t size1;
            char* buffer1;
            File::Load("template2.htmlcpp", buffer1, size1);
            buffer1[size1 - 1] = '\0';
            char* chart1 = new char[size1 + columns1.size() + data1.size() + title1.size() + 50];
            sprintf_s(chart1, size1 + columns1.size() + data1.size() + title1.size() + 50, buffer1, 1, columns1.c_str(), data1.c_str(), title1.c_str(), "Time (ms)", "Algorithm", "chart1_div");
            delete[] buffer1;

            size_t size2;
            char* buffer2;
            File::Load("template2.htmlcpp", buffer2, size2);
            buffer2[size2 - 1] = '\0';
            char* chart2 = new char[size2 + columns2.size() + data2.size() + title2.size() + 50];
            sprintf_s(chart2, size2 + columns2.size() + data2.size() + title2.size() + 50, buffer2, 2, columns2.c_str(), data2.c_str(), title2.c_str(), "Rate (%)", "Algorithm", "chart2_div");
            delete[] buffer2;

            char* charts = new char[strlen(chart1) + strlen(chart2) + 10];
            sprintf_s(charts, strlen(chart1) + strlen(chart2) + 10, "%s\n\n%s\n", chart1, chart2);
            delete[] chart1;
            delete[] chart2;

            size_t sizee;
            char* buffere;
            File::Load("template1.htmlcpp", buffere, sizee);
            buffere[sizee - 1] = '\0';
            char* full = new char[sizee + strlen(charts) + 100];
            sprintf_s(full, sizee + strlen(charts) + 100, buffere, "drawChart1();\ndrawChart2();", charts, "<div id=\"chart1_div\"></div>\n<div id=\"chart2_div\"></div>");
            delete[] buffere;
            delete[] charts;

            std::string filename = "plot_" + benchmarks.begin()->first + ".html";
            File::Save(filename.c_str(), full, strlen(full));
            delete[] full;
        }
        else if (selectedAlgorithms.size() == 1 && vm.count("generator"))
        {
            std::string generator = vm["generator"].as<std::string>();

            std::string title1 = "Comparison of Compression and Decompression Times (lower is better)";

            std::string data1 = "['Size', 'Compression', 'Decompression'],\n";

            int i = 0;
            for (std::vector<BenchmarkData>::iterator itr = benchmarks_gen.begin(); itr != benchmarks_gen.end(); ++itr, ++i)
            {
                if (!itr->IsCompression())
                    continue;

                std::string compressionTime = std::to_string(itr->GetExecutedTime());
                std::string size = std::to_string(itr->GetOriginalSize());
                std::string decompressionTime = std::to_string((++itr)->GetExecutedTime());
                ++i;

                data1 += "['" + size + "', " + compressionTime + ", " + decompressionTime + ']';

                if (i != benchmarks_gen.size() - 1)
                    data1 += ",\n";
                else
                    data1 += '\n';
            }

            std::string title2 = "Comparison of Compression Rates (higher is better)";

            std::string data2 = "['Size', 'Rate'],\n";

            i = 0;
            for (auto& b : benchmarks_gen)
            {
                if (!b.IsCompression()) continue;

                std::string size = std::to_string(b.GetOriginalSize());
                std::string rate = std::to_string(b.GetCompressionRate());

                data2 += "['" + size + "', " + rate + ']';
                if (++i != compressionCount)
                    data2 += ",\n";
                else
                    data2 += '\n';
            }

            // Warning: bad code hereafter

            size_t size1;
            char* buffer1;
            File::Load("template3.htmlcpp", buffer1, size1);
            buffer1[size1 - 1] = '\0';
            char* chart1 = new char[size1 + data1.size() + title1.size() + 50];
            sprintf_s(chart1, size1 + data1.size() + title1.size() + 50, buffer1, 1, data1.c_str(), title1.c_str(), generator == "chars" ?  "Size (chars)" : "Size (words)", "Time (ms)", "", "chart1_div");
            delete[] buffer1;

            size_t size2;
            char* buffer2;
            File::Load("template3.htmlcpp", buffer2, size2);
            buffer2[size2 - 1] = '\0';
            char* chart2 = new char[size2 + data2.size() + title2.size() + 50];
            sprintf_s(chart2, size2 + data2.size() + title2.size() + 50, buffer2, 2, data2.c_str(), title2.c_str(), generator == "chars" ?  "Size (chars)" : "Size (words)", "Rate (%)", "legend: 'none'", "chart2_div");
            delete[] buffer2;

            char* charts = new char[strlen(chart1) + strlen(chart2) + 10];
            sprintf_s(charts, strlen(chart1) + strlen(chart2) + 10, "%s\n\n%s\n", chart1, chart2);
            delete[] chart1;
            delete[] chart2;

            size_t sizee;
            char* buffere;
            File::Load("template1.htmlcpp", buffere, sizee);
            buffere[sizee - 1] = '\0';
            char* full = new char[sizee + strlen(charts) + 100];
            sprintf_s(full, sizee + strlen(charts) + 100, buffere, "drawChart1();\ndrawChart2();", charts, "<div id=\"chart1_div\"></div>\n<div id=\"chart2_div\"></div>");
            delete[] buffere;
            delete[] charts;

            std::string filename = "plot_" + generator + ".html";
            File::Save(filename.c_str(), full, strlen(full));
            delete[] full;
        }
        else
        {
            std::cerr << "Not enough data to generate plots. " << "(c:" << compressionCount << ",d:" << decompressionCount << ",f:" << vm.count("input-file") << ")" << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
