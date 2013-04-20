#include "cute.h"
#include "cute_runner.h"
#include "ide_listener.h"

#undef max

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

Graph<int, double> CreateTestGraph()
{
	Graph<int, double> myGraph;

	for(int i = 1; i < 8; i++)
		myGraph.AddVertex(i);

	myGraph.AddEdge(1 - 1, 2 - 1, 2);
	myGraph.AddEdge(1 - 1, 4 - 1, 7);
	myGraph.AddEdge(2 - 1, 4 - 1, 3);
	myGraph.AddEdge(2 - 1, 5 - 1, 5);
	myGraph.AddEdge(3 - 1, 1 - 1, 2);
	myGraph.AddEdge(3 - 1, 6 - 1, 5);
	myGraph.AddEdge(4 - 1, 3 - 1, 1);
	myGraph.AddEdge(4 - 1, 5 - 1, 1);
	myGraph.AddEdge(4 - 1, 6 - 1, 6);
	myGraph.AddEdge(4 - 1, 7 - 1, 4);
	myGraph.AddEdge(5 - 1, 7 - 1, 2);
	myGraph.AddEdge(6 - 1, 4 - 1, 3);
	myGraph.AddEdge(7 - 1, 6 - 1, 4);

	return myGraph;
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

void test_dijkstra() {
	Graph<int, double> myGraph = CreateTestGraph();

	std::map<uint, uint> shrtsPath = myGraph.dijkstraShortestPath(2);

	vector<uint> vs = myGraph.GetVerticesIds();

	stringstream ss;
	for(unsigned int i = 0; i < vs.size(); i++) {
		int info;
		myGraph.GetElement(i, info);
		ss << info << "<-";
		if ( shrtsPath[i] != std::numeric_limits<uint>::max() )
		{
			int pathInfo;
			myGraph.GetElement(shrtsPath[i], pathInfo);
			ss << pathInfo;
		}
		ss << "|";
	}

	ASSERT_EQUAL("1<-3|2<-1|3<-|4<-2|5<-4|6<-3|7<-5|", ss.str());

	shrtsPath = myGraph.dijkstraShortestPath(0);

	ss.str("");
	for(unsigned int i = 0; i < vs.size(); i++) {
		int info;
		myGraph.GetElement(i, info);
		ss << info << "<-";
		if ( shrtsPath[i] != std::numeric_limits<uint>::max() )
		{
			int pathInfo;
			myGraph.GetElement(shrtsPath[i], pathInfo);
			ss << pathInfo;
		}
		ss << "|";
	}


	ASSERT_EQUAL("1<-|2<-1|3<-4|4<-2|5<-4|6<-4|7<-5|", ss.str());

	//vector<int> path = myGraph.getPath(1, 7);
	//ss.str("");
	//for(unsigned int i = 0; i < path.size(); i++) {
	//	ss << path[i] << " ";
	//}
	//ASSERT_EQUAL("1 2 4 5 7 ", ss.str());

	//myGraph.dijkstraShortestPath(5);
	//path = myGraph.getPath(5, 6);
	//ss.str("");
	//for(unsigned int i = 0; i < path.size(); i++) {
	//	ss << path[i] << " ";
	//}
	//ASSERT_EQUAL("5 7 6 ", ss.str());


	//myGraph.dijkstraShortestPath(7);
	//path = myGraph.getPath(7, 1);
	//ss.str("");
	//for(unsigned int i = 0; i < path.size(); i++) {
	//	ss << path[i] << " ";
	//}
	//ASSERT_EQUAL("7 6 4 3 1 ", ss.str());
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
	s.push_back(CUTE(test_dijkstra));
    ide_listener lis;
    makeRunner(lis)(s, "The Suite");
}

int main(){
    //runSuite();

	Graph<int, int> grp;

	uint elem1 = grp.AddVertex(1);
	uint elem2 = grp.AddVertex(2);
	uint elem3 = grp.AddVertex(3);
	uint elem4 = grp.AddVertex(4);
	uint elem5 = grp.AddVertex(5);
	uint elem6 = grp.AddVertex(6);
	uint elem7 = grp.AddVertex(7);
	uint elem8 = grp.AddVertex(8);
	uint elem9 = grp.AddVertex(9);
	uint elem10 = grp.AddVertex(10);
	uint elem11 = grp.AddVertex(11);
	uint elem12 = grp.AddVertex(12);
	uint elem13 = grp.AddVertex(13);
	uint elem14 = grp.AddVertex(14);

	grp.AddEdge(elem1,  elem2,  1);
	grp.AddEdge(elem1,  elem6,  1);
	grp.AddEdge(elem6,  elem2,  1);
	grp.AddEdge(elem6,  elem7,  1);
	grp.AddEdge(elem5,  elem1,  1);
	grp.AddEdge(elem5,  elem6,  1);
	grp.AddEdge(elem5,  elem11, 1);
	grp.AddEdge(elem6,  elem7,  1);
	grp.AddEdge(elem11, elem12, 1);
	grp.AddEdge(elem7,  elem3,  1);
	grp.AddEdge(elem7,  elem12, 1);
	grp.AddEdge(elem3,  elem4,  1);
	grp.AddEdge(elem3,  elem8,  1);
	grp.AddEdge(elem13, elem12, 1);
	grp.AddEdge(elem13, elem8,  1);
	grp.AddEdge(elem8,  elem14, 1);
	grp.AddEdge(elem4,  elem10, 1);
	grp.AddEdge(elem10, elem9,  1);

	std::vector<uint> topo = grp.topologicalOrder();
	for(const auto& val : topo)
		std::cout << val + 1 << ", ";
	std::cout << std::endl;


	grp.AddEdge(elem2, elem1,   2);
	grp.AddEdge(elem6, elem1,   2);
	grp.AddEdge(elem2, elem6,   2);
	grp.AddEdge(elem7, elem6,   2);
	grp.AddEdge(elem1, elem5,   2);
	grp.AddEdge(elem6, elem5,   2);
	grp.AddEdge(elem11,elem5,   2);
	grp.AddEdge(elem7, elem6,   2);
	grp.AddEdge(elem12,elem11,  2);
	grp.AddEdge(elem3, elem7,   2);
	grp.AddEdge(elem12,elem7,   2);
	grp.AddEdge(elem4, elem3,   2);
	grp.AddEdge(elem8, elem3,   2);
	grp.AddEdge(elem12,elem13,  2);
	grp.AddEdge(elem8, elem13,  2);
	grp.AddEdge(elem14,elem8,   2);
	grp.AddEdge(elem10,elem4,   2);
	grp.AddEdge(elem9, elem10,  2);

	std::map<uint, uint> order = grp.dijkstraShortestPath(4);

	for(const auto& val : order)
		std::cout << val.first + 1 << "|-" << val.second + 1 << ", ";
	std::cout << std::endl;

	order = grp.dijkstraShortestPath(5);

	for(const auto& val : order)
		std::cout << val.first + 1 << "|-" << val.second + 1 << ", ";
	std::cout << std::endl;

	order = grp.dijkstraShortestPath(2);

	for(const auto& val : order)
		std::cout << val.first + 1 << "|-" << val.second + 1 << ", ";
	std::cout << std::endl;

	order = grp.dijkstraShortestPath(7);

	for(const auto& val : order)
		std::cout << val.first + 1 << "|-" << val.second + 1 << ", ";
	std::cout << std::endl;

	order = grp.dijkstraShortestPath(8);

	for(const auto& val : order)
		std::cout << val.first + 1 << "|-" << val.second + 1 << ", ";
	std::cout << std::endl;


    cin.get();
    return 0;
}



