#include "utils.h"

//! Benchmark class
/*!
    Encapsulates information about a benchmark
*/
class Benchmark
{
public:
    Benchmark(uint originalSize, uint resultSize, double startTime, double endTime, bool isCompression)
        : _originalSize(originalSize), _resultSize(resultSize), _startTime(startTime), _endTime(endTime), _compression(isCompression) { } ///< Constructor
    Benchmark() : _originalSize(0), _resultSize(0), _startTime(0), _endTime(0), _compression(false) { } ///< Default Constructor

    void SetOriginalSize(size_t size) { _originalSize = size; }
    void SetResultSize(size_t size) { _resultSize = size; }
    void SetStartTime(double time) { _startTime = time; }
    void SetEndTime(double time) { _endTime = time; }

    void SetCompression(bool val) { _compression = val; }

    double GetExecutedTime() const { return _endTime - _startTime; }
    double GetCompressionRate() const { return (double) _resultSize / (double) _originalSize; }
    bool IsCompression() const { return _compression; }

private:
    double _startTime;
    double _endTime;
    size_t _originalSize;
    size_t _resultSize;
    bool _compression;
};
