#include "utils.h"

class Benchmark
{
public:
    Benchmark(uint originalSize, uint resultSize, double executedTime)
        : _originalSize(originalSize), _resultSize(resultSize), _executedTime(executedTime) { }

    //uint GetOriginalSize() const { return _originalSize; }
    //uint GetResultSize() const { return _resultSize; }
    double GetExecutedTime() const { return _executedTime; }

    double GetCompressionRate() const { return (double) _resultSize / (double) _originalSize; }

private:
    uint _originalSize;
    uint _resultSize;
    double _executedTime;
};
