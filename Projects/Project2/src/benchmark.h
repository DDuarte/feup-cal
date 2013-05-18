#include "utils.h"

class Benchmark
{
public:
    Benchmark(uint originalSize, uint resultSize, double startTime, double endTime, bool isCompression)
        : _originalSize(originalSize), _resultSize(resultSize), _startTime(startTime), _endTime(endTime), _compression(isCompression) { }
    Benchmark() : _originalSize(0), _resultSize(0), _startTime(0), _endTime(0), _compression(false) { }

    void SetOriginalSize(uint size) { _originalSize = size; }
    void SetResultSize(uint size) { _resultSize = size; }
    void SetStartTime(double time) { _startTime = time; }
    void SetEndTime(double time) { _endTime = time; }

    void SetCompression(bool val) { _compression = val; }

    double GetExecutedTime() const { return _endTime - _startTime; }
    double GetCompressionRate() const { return (double) _resultSize / (double) _originalSize; }
    bool IsCompression() const { return _compression; }

private:
    double _startTime;
    double _endTime;
    uint _originalSize;
    uint _resultSize;
    bool _compression;
};
