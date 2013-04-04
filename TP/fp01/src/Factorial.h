/*
 * Factorial.h
 *
 *  Created on: 19 de Fev de 2011
 *      Author: Pedro Silva
 */

#ifndef FACTORIAL_H_
#define FACTORIAL_H_

#include "Defs.h"

/*Calcula o factorial de um valor de entrada n usando recursividade*/
int factorialRecurs(int n);

/*Calcula o factorial de um valor de entrada n usando programação dinâmica*/
int factorialDinam(int n);

/*Calcula o factorial de todos os valores até n (n >=0), colocando os resultados numa array*/
int* factorialDinamArray(int n);

#endif /* FACTORIAL_H_ */
