#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_

#include <cassert>

namespace utils
{

template <class ITR>
class Range
{
public:
    Range(ITR begIt, ITR endIt)
    {
        assert(endIt >= begIt);

        _beg = begIt;
        _end = endIt;
    }

    ITR begin() const { return _beg; }
    ITR end() const { return _end; }
private:
    ITR _beg;
    ITR _end;
};

template <class Fwd>
auto reverse(const Fwd &fwd) -> Range<decltype(fwd.rbegin())> { return Range<decltype(fwd.rbegin())>(fwd.rbegin(), fwd.rend()); }

template <class ITR>
Range<ITR> make_range(ITR begIt, ITR endIt) { return Range<ITR>(begIt, endIt); }

}

#endif // UTILITIES_HPP_
