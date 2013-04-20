/*
 * Change.h
 *
 *  Created on: 19 de Fev de 2011
 *      Author: Pedro Silva
 */

#ifndef CHANGE_H_
#define CHANGE_H_

#include "Defs.h"

/* Calcula o troco para n cêntimos, utilizando o número mínimo de moedas, tendo apenas disponíveis
 * moedas de 1, 2 e 5 cêntimos.
 *
 * Devolve:
 * Uma string que contém com a indicação, para cada n, a combinação de moedas a utilizar.
 * Por exemplo: troco de 1 cêntimo: 1 moeda de 1
 *                 troco de 2 cêntimos: 1 moeda de 2
 *                 troco de 3 cêntimos: 1 moeda de 1, 1 moeda de 2
 *                 (...)
 * A string devolvida representar os valores das moedas (separadas por virgula) para cada n (separado por ponto e vírgula)
 * Exemplo: 1;2;1,2;2,2;5;1,5;...
 * */
string calcChange(int n);

#endif /* CHANGE_H_ */
