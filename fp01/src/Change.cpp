#include "Change.h"

#include <vector>
#include <array>
#include <utility>
#include <sstream>
#include <algorithm>


std::string calcChange( int n )
{
    std::vector<std::pair<int, int> > arr(n+1, make_pair(0,0));
    std::array<size_t,3> coins = { 1, 2, 5 };

    auto coin = coins[0];

    for (size_t k = 1; k < arr.size(); k++)
    {
        arr[k].first = 1 + arr[k - coin].first;
        arr[k].second = coin;
    }

    for (size_t i = 1; i < coins.size(); i++)
    {
        coin = coins[i];
        for (size_t k = coin; k < arr.size(); k++)
        {
            if (1 + arr[k - coin].first < arr[k].first)
            {
                arr[k].first = 1 + arr[k - coin].first;
                arr[k].second = coin;
            }
        }
    }

    std::ostringstream ss;

    for (size_t k = 1; k < arr.size(); k++)
    {
        std::ostringstream sst;
        sst << arr[k].second;
        for (size_t i = k-arr[k].second; i > 0; i -= arr[i].second)
        {
            sst << ",";
            sst << arr[i].second;
        }
        std::string sststr = sst.str();
        std::reverse(sststr.begin(), sststr.end());
        ss << sststr << ";";
    }

    return ss.str();

}
