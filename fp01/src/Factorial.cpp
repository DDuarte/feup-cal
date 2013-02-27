#include "Factorial.h"

#include <vector>
#include <cassert>

int factorialRecurs( int n )
{
    assert(n >= 0);
    if (n <= 1)
        return 1;

    return n * factorialRecurs(n-1);
}

int factorialDinam( int n )
{
    static std::vector<int> factTable;

    if (n == 0)
        return 1;

    if (static_cast<int>(factTable.size()) < n)
        factTable.resize(n, 0);

    if (factTable[n-1] == 0)
        factTable[n-1] = n * factorialDinam(n-1);

    return factTable[n-1];
}

int* factorialDinamArray( int n )
{
    int* res = new int[n+1];

    for (int i = n; i >= 0; i--)
        res[i] = factorialDinam(i);

    return res;
}
