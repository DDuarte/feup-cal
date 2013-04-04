#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#undef max

#include <limits>
#include "Ponto.h"
#include <fstream>
#include <future>
#include <string>
#include <sstream>
#include <tuple>
#include <cmath>

using namespace std;

double nearestPoints_DC(vector<Ponto> &vp, vector<Ponto> &vMP)
{
    vector<const Ponto*> vx(vp.size());
    for (int i = 0; i < vp.size(); i++)
        vx[i] = &vp[i];

    vector<const Ponto*> vy(vx);

    quickSortX(vx);
    quickSortY(vy);

    vector<const Ponto*> vxl, vxr, vyl, vyr;

    const Ponto* vxm = median3(vx, 0, vx.size() - 1, X);
    const Ponto* vym = median3(vx, 0, vx.size() - 1, Y);

    auto vxit = vx.begin();
    while((*vxit)->x < vxm->x) {
        vxl.push_back(*vxit);
        vxit++;
    }
    while (vxit != vx.end()) {
        vxr.push_back(*vxit);
        vxit++;
    }

    auto vyit = vy.begin();
    while((*vyit)->y < vym->y) {
        vyl.push_back(*vyit);
        vyit++;
    }
    while (vyit != vy.end()) {
        vyr.push_back(*vyit);
        vyit++;
    }
    
 

}

double nearestPoints_BF(vector<Ponto> &vp, vector<const Ponto*> &vMP)
{
    double dist = std::numeric_limits<double>::max();
    vMP.resize(2);

    for (const auto& p1 : vp)
    {
        for (const auto& p2 : vp)
        {
            if (&p1 <= &p2)
                break;

            /*if (p1.x - p2.x >= dist || p1.y - p2.y >= dist)
                continue;*/

            double dist1 = p1.distanciaSqr(p2);

            if (dist1 < dist) 
            {
                dist = dist1;
                vMP[0] = &p1;
                vMP[1] = &p2;

                if (dist == 0)
                    return 0.;
            }
        }
    }

    return sqrt(dist);
}


std::string testNearestPoints_BF(string in, double res) {
    std::ostringstream out;
    vector<Ponto> pontos;
    vector<const Ponto*> pontosMP;
    ifstream is(in.c_str());
    lerPontos(is, pontos);

    double d = nearestPoints_BF(pontos, pontosMP);
    out << in << " - Distancia dos pontos mais proximos: " << d << endl;
    out << "p1:" << (*pontosMP[0]) << endl;
    out << "p2:" << (*pontosMP[1]);

    ASSERT_EQUAL_DELTA(res,d,0.01);

    return out.str();
}

std::tuple<std::string, int, int> test(const char* name, double expeced)
{
    int nTimeStart = GetMilliCount();
    std::string res = testNearestPoints_BF(name,expeced);
    int nTimeElapsed = GetMilliSpan( nTimeStart );
    return std::make_tuple(res, nTimeStart, nTimeElapsed);
}

void testNP_BruteForce() {

    auto test1 = std::async(test, "Pontos8", 11841.3);
    auto test2 = std::async(test, "Pontos64", 556.066);
    auto test3 = std::async(test, "Pontos1k", 100.603);
    auto test4 = std::async(test, "Pontos16k", 13.0384);
    auto test5 = std::async(test, "Pontos32k", 1);
    auto test6 = std::async(test, "Pontos64k", 1);
    auto test7 = std::async(test, "Pontos128k", 0);
    
    auto t = test1.get();
    cout << "t1" << endl;
    cout << std::get<0>(t) << endl;
    cout << std::get<2>(t) << endl << endl;
    t = test2.get();
    cout << "t2" << endl;
    cout << std::get<0>(t) << endl;
    cout << std::get<2>(t) << endl << endl;
    t = test3.get();
    cout << "t3" << endl;
    cout << std::get<0>(t) << endl;
    cout << std::get<2>(t) << endl << endl;
    t = test4.get();
    cout << "t4" << endl;
    cout << std::get<0>(t) << endl;
    cout << std::get<2>(t) << endl << endl;
    t = test5.get();
    cout << "t5" << endl;
    cout << std::get<0>(t) << endl;
    cout << std::get<2>(t) << endl << endl;
    t = test6.get();
    cout << "t6" << endl;
    cout << std::get<0>(t) << endl;
    cout << std::get<2>(t) << endl << endl;
    t = test7.get();
    cout << "t7" << endl;
    cout << std::get<0>(t) << endl;
    cout << std::get<2>(t) << endl << endl;
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
    /*int nTimeStart = GetMilliCount();
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
    cout << nTimeElapsed << endl;*/
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
    cin.get();
    return 0;
}



