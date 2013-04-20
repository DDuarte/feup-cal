/*
 * Factorial.h
 *
 *  Created on: 19 de Fev de 2011
 *      Author: Pedro Silva
 */

#ifndef FACTORIAL_H_
#define FACTORIAL_H_

#include "Defs.h"

#include <array>

/*Calcula o factorial de um valor de entrada n usando recursividade*/
int factorialRecurs(int n)
{
    assert(n >= 0);

    if (n == 1 || n == 0)
        return 1;
    else
        return n * factorialRecurs(n - 1);
}

/*Calcula o factorial de um valor de entrada n usando programação dinâmica*/
int factorialDinam(int n)
{
    assert(n >= 0 && n <= 199);
    static std::array<int, 200> arr;

    if (arr[n] > 1)
        return arr[n];

    if (n == 1 || n == 0)
        return 1;

    arr[n] = n * factorialDinam(n - 1);

    return arr[n];
}

/*Calcula o factorial de todos os valores até n (n >=0), colocando os resultados numa array*/
int* factorialDinamArray(int n)
{
    assert(n >= 0);
    int* arr = new int[n + 1];

    for (int i = n; i >= 0; --i)
        arr[i] = factorialDinam(i);

    return arr;
}

#endif /* FACTORIAL_H_ */
