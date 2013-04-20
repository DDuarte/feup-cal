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

    Ponto();
    Ponto(double x, double y);
    Ponto(int x, int y);
    double distancia(Ponto &p);
    double distanciaSqr(const Ponto &p) const
    {
        return (pow(x-p.x, 2) + pow(y-p.y, 2));
    }
    virtual ~Ponto();
    bool operator==(const Ponto &p) const;
};
ostream& operator<<(ostream& os, const Ponto &p);

/*
 * constantes e funcoes genericas
 */
const int X=0;
const int Y=1;

void lerPontos(istream &is, vector<Ponto> &vp);

double comparaPontos(Ponto p1, Ponto p2, int type);
double comparaPontos(const Ponto* p1, const Ponto* p2, int type);

template <typename T>
void quickSortX(vector<T> &vp);
template <typename T>
void quickSortY(vector<T> &vp);
template <typename T>
void quickSort(vector<T> &v, int left, int right, int type);

template <typename T>
T& median3(vector<T> &v, int left, int right, int type);
template <typename T>
void insertionSort(vector<T> &v, int left, int right, int type);

//---------------------------------------------------------------------------
/**
 * quickSort algorithm for 'std::vector<bestHistory>' by ascending X coordinate
 */
template <typename T>
void quickSortX(vector<T> &vp)
{
    quickSort(vp, 0, static_cast<int>(vp.size()-1), X);
}
//---------------------------------------------------------------------------

/**
 * quickSort algorithm for 'std::vector<bestHistory>' by ascending Y coordinate
 */
template <typename T>
void quickSortY(vector<T> &vp)
{
    quickSort(vp, 0, static_cast<int>(vp.size()-1), Y);
}
//---------------------------------------------------------------------------
template <typename T>
void quickSort(vector<T> &v, int left, int right, int type)
{
    if (right - left <= 50)    // if small vector
        insertionSort(v, left, right, type);
    else {
        T p = median3(v, left, right, type);  // x is pivot
        int i = left; int j = right-1;            // partition step
        double comp;
        for(; ; ) {

            comp = -1;
            while (comp < 0) {
                comp = comparaPontos(v[++i], p, type);
            }

            comp = -1;
            while (comp < 0) {
                comp = comparaPontos(p, v[--j], type);
            }
            if (i < j)
                swap(v[i], v[j]);
            else break;
        }
        swap(v[i], v[right-1]);  // reset pivot
        quickSort(v, left, i-1, type);
        quickSort(v, i+1, right, type);
    }
}
//---------------------------------------------------------------------------

/**
 * choose pivot as the median of three values:
 * extremes and central value of vector
 */
template <typename T>
T & median3(vector<T> &v, int left, int right, int type)
{
    int center = (left+right) /2;
    if (comparaPontos(v[center], v[left], type) < 0)
        swap(v[left], v[center]);

    if (comparaPontos(v[right], v[left], type) < 0)
        swap(v[left], v[right]);

    if (comparaPontos(v[right], v[center], type) < 0)
        swap(v[center], v[right]);

    swap(v[center], v[right-1]);     // put pivot in position 'right-1'
    return v[right-1];
}
//---------------------------------------------------------------------------

/**
 * for small vectors use a simple method - insertion sort
 */
template <typename T>
void insertionSort(vector<T> &v, int left, int right, int type)
{
    for (int i = left+1; i < right+1; i++)
    {
        T tmp = v[i];
        int j;
        for (j = i; j > left && comparaPontos(tmp, v[j-1], type) < 0; j--)
            v[j] = v[j-1];
        v[j] = tmp;
    }
}

int GetMilliCount();
int GetMilliSpan(int nTimeStart);

/*
 * funcao a implementar
 */
double pontosMaisProximos(vector<Ponto> &v, vector<Ponto> &vMP);

#endif /* PONTO_H_ */
