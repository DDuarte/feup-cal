#include "Sum.h"

#include <vector>
#include <utility>
#include <algorithm>

std::string calcSum( int* sequence, int size )
{
    std::vector<int> seq(sequence, sequence + size);
    std::vector<int> sum(seq);
    std::vector<std::pair<int, int>> res(size);

    for (int i = 0; i < seq.size(); i++)
    {
        auto sumEnd = sum.end() - i;
        auto minElem = std::min_element(sum.begin(), sumEnd);
        res[i] = make_pair(*minElem, std::distance(sum.begin(), minElem));

        for (auto sumit = sum.begin(), seqit = seq.begin() + i + 1; seqit != seq.end(); sumit++, seqit++)
        {
            *sumit += *seqit;
        }
    }

    std::ostringstream ss;

    for (auto r : res)
        ss << r.first << "," << r.second << ";";
    

    return ss.str();
}
