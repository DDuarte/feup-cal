#include "Partitioning.h"

#include <vector>
#include <map>
#include <cassert>

int s_recursive( int n,int k )
{
    assert(n > 0);
    assert(1 <= k && k <= n);

    if (k == 1 || k == n)
        return 1;

    return s_recursive(n-1, k-1) + k * s_recursive(n-1, k);
}

int b_recursive( int n )
{
    int sum = 0;

    for (int k = 1; k <= n; k++)
        sum += s_recursive(n, k);

    return sum;
}

int s_dynamic( int n, int k )
{
    if (k == 1 || k == n)
        return 1;

    std::vector<int> vec(n-k+1, 1);

    for (int i = 2; i <= k; i++)
        for (int j = 1; j < vec.size(); j++)
            vec[j] += i* vec[j-1];

    return vec[vec.size()-1];
}

int b_dynamic( int n )
{
    int sum = 0;

    static std::map<int, int> storage;

    auto elem = storage.find(n);
    if (elem != storage.end())
        return elem->second;

    for (int k = 1; k <= n; k++)
        sum += s_dynamic(n, k);

    storage[n] = sum;

    return sum;
}
