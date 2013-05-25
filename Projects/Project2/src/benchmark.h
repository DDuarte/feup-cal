#include "utils.h"

//! Benchmark class
/*!
    Encapsulates information about a benchmark
*/
class Benchmark
{
public:
    Benchmark(uint originalSize, uint resultSize, double startTime, double endTime, bool isCompression, const std::string& alg)
        : _originalSize(originalSize), _resultSize(resultSize), _startTime(startTime), _endTime(endTime), _compression(isCompression), _algorithm(alg) { } ///< Constructor
    Benchmark() : _originalSize(0), _resultSize(0), _startTime(0), _endTime(0), _compression(false) { } ///< Default Constructor

    void SetOriginalSize(size_t size) { _originalSize = size; }
    void SetResultSize(size_t size) { _resultSize = size; }
    void SetStartTime(double time) { _startTime = time; }
    void SetEndTime(double time) { _endTime = time; }
    void SetCompression(bool val) { _compression = val; }
    void SetAlgorithm(const std::string& alg) { _algorithm = alg; }

    size_t GetOriginalSize() const { return _originalSize; }
    size_t GetResultSize() const { return _resultSize; }
    double GetStartTime() const { return _startTime; }
    double GetEndTime() const { return _endTime; }
    bool IsCompression() const { return _compression; }
    const std::string& GetAlgorithm() const { return _algorithm; }

    double GetExecutedTime() const { return (_endTime - _startTime) * 1000.0; } ///< (De)Compression execution time in milliseconds
    double GetCompressionRate() const { return (1.0 - ((double) _resultSize / (double) _originalSize)) * 100.0; }

private:
    std::string _algorithm;
    double _startTime;
    double _endTime;
    size_t _originalSize;
    size_t _resultSize;
    bool _compression;
};
