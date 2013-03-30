#include "cute.h"
#include "cute_runner.h"
#include "ide_listener.h"

#include <iostream>

#include "defs.h"
#include "graph.h"
#include "Person.h"

using namespace cute;
using namespace std;

void createNetwork(Graph<Person, double> & net1)
{
	uint p1 = net1.AddVertex(Person("Ana"		, 19));		// 0
	uint p2 = net1.AddVertex(Person("Carlos"	, 33));		// 1
	uint p3 = net1.AddVertex(Person("Filipe"	, 20));		// 2
	uint p4 = net1.AddVertex(Person("Ines"		, 18));		// 3
	uint p5 = net1.AddVertex(Person("Maria"		, 24));		// 4
	uint p6 = net1.AddVertex(Person("Rui"		, 21));		// 5
	uint p7 = net1.AddVertex(Person("Vasco"		, 28));		// 6
	net1.AddEdge(p1,p2,0);
	net1.AddEdge(p1,p3,0);
	net1.AddEdge(p1,p4,0);
	net1.AddEdge(p2,p5,0);
	net1.AddEdge(p5,p6,0);
	net1.AddEdge(p5,p1,0);
	net1.AddEdge(p3,p6,0);
	net1.AddEdge(p3,p7,0);
	net1.AddEdge(p6,p2,0);
}

void test_AddVertex()
{
	Graph<Person, double> net1;
	uint p1 = net1.AddVertex(Person("Ana"		, 19));
	uint p2 = net1.AddVertex(Person("Carlos"	, 33));
	uint p3 = net1.AddVertex(Person("Filipe"	, 20));
	uint p4 = net1.AddVertex(Person("Ines"		, 18));
	ASSERT_EQUAL(4, net1.GetNumberOfVertices());
}

void test_RemoveVertex()
{
	Graph<Person, double> net1;
	uint p1 = net1.AddVertex(Person("Ana"		, 19));
	uint p2 = net1.AddVertex(Person("Carlos"	, 33));
	uint p3 = net1.AddVertex(Person("Filipe"	, 20));
	uint p4 = net1.AddVertex(Person("Ines"		, 18));
	ASSERT_EQUAL(true, net1.RemoveVertex(p2));
	ASSERT_EQUAL(false, net1.RemoveVertex(p2));
	ASSERT_EQUAL(3, net1.GetNumberOfVertices());
}

void test_AddEdge()
{
	Graph<Person, double> net1;
	uint p1 = net1.AddVertex(Person("Ana"		, 19));
	uint p2 = net1.AddVertex(Person("Carlos"	, 33));
	uint p3 = net1.AddVertex(Person("Filipe"	, 20));
	uint p4 = net1.AddVertex(Person("Ines"		, 18));
	uint p5 = p4 + 1;
	ASSERT_EQUAL(true, net1.AddEdge(p1,p2,0));
	ASSERT_EQUAL(true, net1.AddEdge(p1,p3,0));
	ASSERT_EQUAL(true, net1.AddEdge(p1,p4,0));
	ASSERT_EQUAL(false, net1.AddEdge(p2,p5,0));
}

void test_RemoveEdge() 
{
	Graph<Person, double> net1;
	uint p1 = net1.AddVertex(Person("Ana"		, 19));
	uint p2 = net1.AddVertex(Person("Carlos"	, 33));
	uint p3 = net1.AddVertex(Person("Filipe"	, 20));
	uint p4 = net1.AddVertex(Person("Ines"		, 18));
	uint p5 = p4 + 1;
	ASSERT_EQUAL(true, net1.AddEdge(p1,p2,0));
	ASSERT_EQUAL(true, net1.AddEdge(p1,p3,0));
	ASSERT_EQUAL(true, net1.AddEdge(p1,p4,0));
	ASSERT_EQUAL(true, net1.AddEdge(p2,p4,0));
	ASSERT_EQUAL(true, net1.RemoveEdge(p1,p3));
	ASSERT_EQUAL(false, net1.RemoveEdge(p1,p5));
	ASSERT_EQUAL(false, net1.RemoveEdge(p2,p3));
}

void MyTest()
{
    Graph<int, double> gr;
	ASSERT_EQUAL(0, gr.AddVertex(10));
	ASSERT_EQUAL(1, gr.AddVertex(11));
	ASSERT_EQUAL(false, gr.RemoveVertex(2));
	ASSERT_EQUAL(true, gr.AddEdge(0, 1, 19.0));
	ASSERT_EQUAL(false, gr.RemoveEdge(1, 0));
	ASSERT_EQUAL(true, gr.RemoveEdge(0, 1));
	ASSERT_EQUAL(true, gr.RemoveVertex(1));
	ASSERT_EQUAL(true, gr.RemoveVertex(0));
	ASSERT_EQUAL(false, gr.RemoveVertex(1));
}

void test_dfs()
{
	Graph<Person, double> net1;
	createNetwork(net1);
	std::vector<uint> v1=net1.dfs();
	ASSERT_EQUAL(0, v1[0]);
	ASSERT_EQUAL(1, v1[1]);
	ASSERT_EQUAL(4, v1[2]);
	ASSERT_EQUAL(5, v1[3]);
	ASSERT_EQUAL(2, v1[4]);
	ASSERT_EQUAL(3, v1[5]);
	ASSERT_EQUAL(6, v1[6]);
}


void runSuite(){
    cute::suite s;
    //TODO add your test here
    s.push_back(CUTE(test_AddVertex));
	s.push_back(CUTE(test_RemoveVertex));
	s.push_back(CUTE(test_AddEdge));
	s.push_back(CUTE(test_RemoveEdge));
    ide_listener lis;
    makeRunner(lis)(s, "The Suite");
}

int main(){
    runSuite();
    cin.get();
    return 0;
}



