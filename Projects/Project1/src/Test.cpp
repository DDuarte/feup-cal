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
	ASSERT_EQUAL(6, v1[5]);
	ASSERT_EQUAL(3, v1[6]);
}

void test_d_topologicaOrder() {
	Graph<int, void*> myGraph;
	uint elem1 = myGraph.AddVertex(1);
	uint elem2 = myGraph.AddVertex(2); 
	uint elem3 = myGraph.AddVertex(3); 
	uint elem4 = myGraph.AddVertex(4);
	uint elem5 = myGraph.AddVertex(5); 
	uint elem6 = myGraph.AddVertex(6); 
	uint elem7 = myGraph.AddVertex(7);

	myGraph.AddEdge(elem1, elem2, nullptr);
	myGraph.AddEdge(elem1, elem4, nullptr);
	myGraph.AddEdge(elem1, elem3, nullptr);
	myGraph.AddEdge(elem2, elem5, nullptr);
	myGraph.AddEdge(elem2, elem4, nullptr);
	myGraph.AddEdge(elem3, elem6, nullptr);
	myGraph.AddEdge(elem4, elem3, nullptr);
	myGraph.AddEdge(elem4, elem6, nullptr);
	myGraph.AddEdge(elem4, elem7, nullptr);
	myGraph.AddEdge(elem5, elem4, nullptr);
	myGraph.AddEdge(elem5, elem7, nullptr);
	myGraph.AddEdge(elem7, elem6, nullptr);

	vector<uint> topOrder;

	topOrder = myGraph.topologicalOrder();
	stringstream ss;
	for( unsigned int i = 0; i < topOrder.size(); i++) 
	{
		int elem;
		myGraph.GetElement(topOrder[i], elem);
		ss << elem << " ";
	}
	ASSERT_EQUAL("1 2 5 4 3 7 6 ", ss.str());

	vector<uint> vertices = myGraph.GetVerticesIds();
	ss.str("");
	for (unsigned int i = 0; i < vertices.size(); i++){
		int info, indegree;
		myGraph.GetElement(vertices[i], info);
		myGraph.GetElementInDegree(vertices[i], indegree);
		ss << info << "(" << indegree << ") ";
	}
	ASSERT_EQUAL("1(0) 2(1) 3(2) 4(3) 5(1) 6(3) 7(2) ", ss.str());

	myGraph.AddEdge(elem3, elem1, 0);

	topOrder = myGraph.topologicalOrder();
	ss.str("");
	for( unsigned int i = 0; i < topOrder.size(); i++) 
	{
		int elem;
		myGraph.GetElement(topOrder[i], elem);
		ss << elem << " ";
	}
	ASSERT_EQUAL("", ss.str());
}

void runSuite(){
    cute::suite s;
    //TODO add your test here
    s.push_back(CUTE(test_AddVertex));
	s.push_back(CUTE(test_RemoveVertex));
	s.push_back(CUTE(test_AddEdge));
	s.push_back(CUTE(test_RemoveEdge));
	s.push_back(CUTE(test_dfs));
	s.push_back(CUTE(test_d_topologicaOrder));
    ide_listener lis;
    makeRunner(lis)(s, "The Suite");
}

int main(){
    runSuite();
    cin.get();
    return 0;
}



