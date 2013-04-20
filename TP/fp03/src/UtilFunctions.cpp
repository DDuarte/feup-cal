/*
 * UtilFunctions.cpp
 */

#include "Ponto.h"
#include <time.h>
#include <sys/timeb.h>

/**
 * function to fill list 'lp' with 'Ponto' objects read from 'is'
 */
void lerPontos(istream &is, vector<Ponto> &vp){
    double x, y;
    if (!is)
        return;
    while (!is.eof()) {
        is >> x >> y;
        Ponto p(x,y);
        vp.push_back(p);
    }
}
//---------------------------------------------------------------------------

/**
 * function to compare 'Ponto' based on 'X' or 'Y'
 */
double comparaPontos(Ponto p1, Ponto p2, int type) {
    if (type == X)
        return p1.x - p2.x;
    return p1.y - p2.y;
}

/**
 * function to compare 'Ponto' based on 'X' or 'Y'
 */
double comparaPontos(const Ponto* p1, const Ponto* p2, int type) {
    if (type == X)
        return p1->x - p2->x;
    return p1->y - p2->y;
}

//---------------------------------------------------------------------------

/*
 * Print time
 */
// Something like GetTickCount but portable
// It rolls over every ~ 12.1 days (0x100000/24/60/60)
// Use GetMilliSpan to correct for rollover

int GetMilliCount()
{
  timeb tb;
  ftime( &tb );
  int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
  return nCount;
}
//---------------------------------------------------------------------------

int GetMilliSpan(int nTimeStart)
{
  int nSpan = GetMilliCount() - nTimeStart;
  if (nSpan < 0)
      nSpan += 0x100000 * 1000;
  return nSpan;
}
//---------------------------------------------------------------------------

