#include "../cute/cute.h"
#include "../cute/ide_listener.h"
#include "../cute/cute_runner.h"
#include "graph.hpp"
#include <sstream>

using namespace std;

Graph<int, double> CreateTestGraph()
{
	Graph<int, double> myGraph;

	for(int i = 1; i < 8; i++)
		myGraph.addVertex(i);

	myGraph.addEdge(1, 2, 2);
	myGraph.addEdge(1, 4, 7);
	myGraph.addEdge(2, 4, 3);
	myGraph.addEdge(2, 5, 5);
	myGraph.addEdge(3, 1, 2);
	myGraph.addEdge(3, 6, 5);
	myGraph.addEdge(4, 3, 1);
	myGraph.addEdge(4, 5, 1);
	myGraph.addEdge(4, 6, 6);
	myGraph.addEdge(4, 7, 4);
	myGraph.addEdge(5, 7, 2);
	myGraph.addEdge(6, 4, 3);
	myGraph.addEdge(7, 6, 4);

	return myGraph;
}


void test_bellmanFord() {
	Graph<int, double> myGraph = CreateTestGraph();

	//para testar o metodo unweightedShortestPath
	myGraph.bellmanFordShortestPath(3);

	vector<int> vs = myGraph.getVertices();

	stringstream ss;
	for(unsigned int i = 0; i < vs.size(); i++) {
		ss << vs[i] << "<-";
		int path;
		
		if (myGraph.getVertexPath(vs[i], path))  ss << path;
		ss << "|";
	}

	ASSERT_EQUAL("1<-3|2<-1|3<-|4<-2|5<-4|6<-3|7<-5|", ss.str());

	myGraph.bellmanFordShortestPath(1);
	
	vector<int> path = myGraph.getPath(1, 7);
	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}
	ASSERT_EQUAL("1 2 4 5 7 ", ss.str());

	myGraph.bellmanFordShortestPath(5);
	path = myGraph.getPath(5, 6);
	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}
	ASSERT_EQUAL("5 7 6 ", ss.str());


	myGraph.bellmanFordShortestPath(7);
	path = myGraph.getPath(7, 1);
	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}
	ASSERT_EQUAL("7 6 4 3 1 ", ss.str());
}

void test_dijkstra() {
	Graph<int, double> myGraph = CreateTestGraph();

	//para testar o metodo unweightedShortestPath
	myGraph.dijkstraShortestPath(3);

	vector<int> vs = myGraph.getVertices();

	stringstream ss;
	for(unsigned int i = 0; i < vs.size(); i++) {
		ss << vs[i] << "<-";
		int path;
		if ( myGraph.getVertexPath(vs[i], path))  ss << path;
		ss << "|";
	}

	ASSERT_EQUAL("1<-3|2<-1|3<-|4<-2|5<-4|6<-3|7<-5|", ss.str());

	myGraph.dijkstraShortestPath(1);

	ss.str("");
	for(unsigned int i = 0; i < vs.size(); i++) {
		ss << vs[i] << "<-";
		int path;
		if (myGraph.getVertexPath(vs[i], path))  ss << path;
		ss << "|";
	}

	ASSERT_EQUAL("1<-|2<-1|3<-4|4<-2|5<-4|6<-4|7<-5|", ss.str());

	vector<int> path = myGraph.getPath(1, 7);
	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}
	ASSERT_EQUAL("1 2 4 5 7 ", ss.str());

	myGraph.dijkstraShortestPath(5);
	path = myGraph.getPath(5, 6);
	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}
	ASSERT_EQUAL("5 7 6 ", ss.str());


	myGraph.dijkstraShortestPath(7);
	path = myGraph.getPath(7, 1);
	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}
	ASSERT_EQUAL("7 6 4 3 1 ", ss.str());*/
}

void test_floydWarshall() {
	/*Graph<int> myGraph = CreateTestGraph();

	//para testar o metodo unweightedShortestPath
	myGraph.floydWarshallShortestPath();

	stringstream ss;

	vector<int> path = myGraph.getfloydWarshallPath(1, 7);
	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}
	ASSERT_EQUAL("1 2 4 5 7 ", ss.str());

	path = myGraph.getfloydWarshallPath(5, 6);
	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}
	ASSERT_EQUAL("5 7 6 ", ss.str());


	path = myGraph.getfloydWarshallPath(7, 1);
	ss.str("");
	for(unsigned int i = 0; i < path.size(); i++) {
		ss << path[i] << " ";
	}
	ASSERT_EQUAL("7 6 4 3 1 ", ss.str());*/
}

void runSuite(){
	cute::suite s;
	s.push_back(CUTE(test_bellmanFord));
	s.push_back(CUTE(test_dijkstra));
	s.push_back(CUTE(test_floydWarshall));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "CAL 2012/2013 - Aula Pratica 6");
}

int main(){
    runSuite();
	std::cin.get();
    return 0;
}


