#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"


#undef max

#include "Ponto.h"
#include <fstream>
#include <numeric>
#include <limits>

using namespace std;

//double nearestPoints_DC(vector<Ponto> &vp, vector<Ponto> &vMP);
//double nearestPoints_BF(vector<Ponto> &vp, vector<Ponto> &vMP);

double nearestPoints_BF(const vector<Ponto> &vp, vector<const Ponto*> &vMP)
{
    double minDist = std::numeric_limits<double>::max();
    vMP.resize(2);

    for (const auto& p1 : vp)
    {
        for (const auto& p2 : vp)
        {
            if (&p1 <= &p2)
                continue;

            double dist = p1.distanciaSqr(p2);
            if (dist < minDist)
            {
                vMP[0] = &p1;
                vMP[1] = &p2;
                minDist = dist;

                if (dist == 0)
                    return 0;
            }
        }
    }

    return sqrt(minDist);
}

double nearestPoints_DC(const vector<Ponto> &vp, vector<Ponto> &vMP)
{

    vector<Ponto> vx(vp);
    vector<Ponto> vy(vp);

    quickSortX(vx);
    quickSortY(vy);

    vector<Ponto> vxl, vxy, vyl, 
}

void testNearestPoints_BF(string in, double res) {
	vector<Ponto> pontos;
    vector<const Ponto*> pontosMP;
	ifstream is(in.c_str());
	lerPontos(is, pontos);

	double d = nearestPoints_BF(pontos, pontosMP);
	cout << in << " - Distancia dos pontos mais proximos: " << d << endl;
	cout << "p1:" << (*pontosMP[0]) << endl;
	cout << "p2:" << (*pontosMP[1]) << endl;

	ASSERT_EQUAL_DELTA(res,d,0.01);
}

void testNP_BruteForce() {
	int nTimeStart = GetMilliCount();
	cout << nTimeStart << endl;
	testNearestPoints_BF("Pontos8",11841.3);
	int nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_BF("Pontos64",556.066);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_BF("Pontos1k",100.603);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_BF("Pontos16k",13.0384);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_BF("Pontos32k",1);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_BF("Pontos64k",1);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_BF("Pontos128k",0);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;
}



void testNearestPoints_DC(string in, double res) {
	/*vector<Ponto> pontos, pontosMP;
	ifstream is(in.c_str());
	lerPontos(is, pontos);

	double d = nearestPoints_DC(pontos, pontosMP);
	cout << in << " - Distancia dos pontos mais proximos: " << d << endl;
	cout << "p1:" << pontosMP[0] << endl;
	cout << "p2:" << pontosMP[1] << endl;

	ASSERT_EQUAL_DELTA(res,d,0.01);*/
}

void testNP_DivideConquer() {
	int nTimeStart = GetMilliCount();
	cout << nTimeStart << endl;
	testNearestPoints_DC("Pontos8",11841.3);
	int nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_DC("Pontos64",556.066);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_DC("Pontos1k",100.603);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_DC("Pontos16k",13.0384);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_DC("Pontos32k",1);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_DC("Pontos64k",1);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;

	nTimeStart += nTimeElapsed;
	testNearestPoints_DC("Pontos128k",0);
	nTimeElapsed = GetMilliSpan( nTimeStart );
	cout << nTimeElapsed << endl;
}

void runSuite(){
	cute::suite s;
	//TODO add your test here
	s.push_back(CUTE(testNP_BruteForce));
	s.push_back(CUTE(testNP_DivideConquer));
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
}

int main(){
    runSuite();
    system("PAUSE");
    return 0;
}



