/*
 * Ponto.h
 */

#ifndef PONTO_H_
#define PONTO_H_

#include <iostream>
#include <vector>

using namespace std;

class Ponto {
public:
    double x;
    double y;

    Ponto() { };
    Ponto(double x, double y) : x(x), y(y) { }

    Ponto(int x, int y) : x(x), y(y) { }
    double distanciaSqr(const Ponto &p) const { return (pow(x-p.x, 2) + pow(y-p.y, 2));  }
    double distancia(const Ponto &p) const { return (sqrt(pow(x-p.x, 2) + pow(y-p.y, 2))); }
    bool operator==(const Ponto &p) const { return (x == p.x && y == p.y); }
};
ostream& operator<<(ostream& os, const Ponto &p);

/*
 * constantes e funcoes genericas
 */
const int X=0;
const int Y=1;

void lerPontos(istream &is, vector<Ponto> &vp);
double comparaPontos(Ponto p1, Ponto p2, int type);

void quickSortX(vector<Ponto> &vp);
void quickSortY(vector<Ponto> &vp);
void quickSort(vector<Ponto> &v, int left, int right, int type);
Ponto& median3(vector<Ponto> &v, int left, int right, int type);
void insertionSort(vector<Ponto> &v, int left, int right, int type);

int GetMilliCount();
int GetMilliSpan(int nTimeStart);

/*
 * funcao a implementar
 */
double pontosMaisProximos(vector<Ponto> &v, vector<Ponto> &vMP);

#endif /* PONTO_H_ */
