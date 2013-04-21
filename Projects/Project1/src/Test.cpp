#include "cute.h"
#include "cute_runner.h"
#include "ide_listener.h"

#undef max

#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <ctime>

#include "defs.h"
#include "graph.h"
#include "hydographic_network.h"

Graph<int,int> Load(const char* fileName)
{
	Graph<int,int> result;
	std::ifstream file(fileName);

	if (!file.is_open())
		return result;

	int numberVertex;
	file >> numberVertex;

	for (int i = 0; i < numberVertex; ++i)
		result.AddVertex(i);

	int numberEdges;
	file >> numberEdges;

	for (int i = 0; i < numberEdges; ++i)
	{
		int id1, id2;
		file >> id1 >> id2;
		result.AddEdge(id1, id2, 1);
	}

	return result;
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

std::vector<uint> GetPath(const std::map<uint, std::pair<uint,double>>& dsp, uint src, uint dst)
{
	std::vector<uint> result;

	uint act = dst;

	while(act != -1 && act != src)
	{
		result.push_back(act);
		act = dsp.at(act).first;
	}

	if (act == -1)
		return std::vector<uint>();
	else
		result.push_back(act);

	std::reverse(result.begin(), result.end());
	return result;
}
//
///*
//void test_dijkstra() {
//    Graph<int, double> myGraph = CreateTestGraph();
//
//    std::map<uint, uint> shrtsPath = myGraph.dijkstraShortestPath(2);
//
//    vector<uint> vs = myGraph.GetVerticesIds();
//
//    stringstream ss;
//    for(unsigned int i = 0; i < vs.size(); i++) {
//        int info;
//        myGraph.GetElement(i, info);
//        ss << info << "<-";
//        if ( shrtsPath[i] != std::numeric_limits<uint>::max() )
//        {
//            int pathInfo;
//            myGraph.GetElement(shrtsPath[i], pathInfo);
//            ss << pathInfo;
//        }
//        ss << "|";
//    }
//
//    ASSERT_EQUAL("1<-3|2<-1|3<-|4<-2|5<-4|6<-3|7<-5|", ss.str());
//
//    shrtsPath = myGraph.dijkstraShortestPath(0);
//
//    ss.str("");
//    for(unsigned int i = 0; i < vs.size(); i++) {
//        int info;
//        myGraph.GetElement(i, info);
//        ss << info << "<-";
//        if ( shrtsPath[i] != std::numeric_limits<uint>::max() )
//        {
//            int pathInfo;
//            myGraph.GetElement(shrtsPath[i], pathInfo);
//            ss << pathInfo;
//        }
//        ss << "|";
//    }
//
//
//    ASSERT_EQUAL("1<-|2<-1|3<-4|4<-2|5<-4|6<-4|7<-5|", ss.str());
//
//        vector<uint> path = GetPath(shrtsPath, 0, 6);
//        ss.str("");
//        for(unsigned int i = 0; i < path.size(); i++) {
//                ss << path[i] + 1 << " ";
//        }
//        ASSERT_EQUAL("1 2 4 5 7 ", ss.str());
//
//    //myGraph.dijkstraShortestPath(5);
//    //path = myGraph.getPath(5, 6);
//    //ss.str("");
//    //for(unsigned int i = 0; i < path.size(); i++) {
//    //    ss << path[i] << " ";
//    //}
//    //ASSERT_EQUAL("5 7 6 ", ss.str());
//	//
//	//
//    //myGraph.dijkstraShortestPath(7);
//    //path = myGraph.getPath(7, 1);
//    //ss.str("");
//    //for(unsigned int i = 0; i < path.size(); i++) {
//    //    ss << path[i] << " ";
//    //}
//    //ASSERT_EQUAL("7 6 4 3 1 ", ss.str());
//}
//*/
//void runSuite(){
//    cute::suite s;
//    //TODO add your test here
//    s.push_back(CUTE(test_d_topologicaOrder));
////    s.push_back(CUTE(test_dijkstra));
//    ide_listener lis;
//    makeRunner(lis)(s, "The Suite");
//}

int main(){
	srand(time(nullptr));
	//runSuite();

	Village v1("A");
	Village v2("B");
	Village v3("C");
	Village v4("D");
	Village v5("E");

	River r1("R1", 100);
	River r2("R2", 100);
	River r3("R3", 50);
	River r4("R4", 150);

	HydographicNetwork hn;
	uint a1 = hn.AddVillage(v1);
	uint a2 = hn.AddVillage(v2);
	uint a3 = hn.AddVillage(v3);
	uint a4 = hn.AddVillage(v4);
	uint a5 = hn.AddVillage(v5);

	hn.AddRiver(a1, a2, r1);
	hn.AddRiver(a1, a3, r2);
	hn.AddRiver(a3, a4, r3);
	hn.AddRiver(a4, a5, r4);

	/*hn.ViewGraph();*/

	std::multimap<uint, Order> orders;

	orders.insert(std::make_pair(a1, Order(100000., 0.)));
	orders.insert(std::make_pair(a5, Order(100., 0.)));
	orders.insert(std::make_pair(a2, Order(1000., 0.)));

	HydographicNetwork::Delivery del = hn.GetDeliveryPath(a1, orders);





	/*

	Graph<int, int> grp;
	//Graph<int, int> grp = Load("graph.txt");

	//for (uint elem : topo)
	//	std::cout << elem << ", ";
	//std::cout << std::endl;



	//std::map<uint,uint> d1 = grp.dijkstraShortestPath(0);
	//
	//std::vector<uint> path = GetPath(d1, 0, 67);
	//
	//for (uint elem : path)
	//{
	//	std::cout << elem << "->";
	//}
	//std::cout << std::endl;
	//
	//for(const auto& val : d1)
	//	std::cout << val.first + 1 << "|-" << val.second + 1 << ", ";
	//std::cout << std::endl;

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

	uint src = 4;

	std::vector<uint> topo = grp.topologicalOrder();

	for (uint elem: topo)
	std::cout << elem + 1<< ", ";
	std::cout << std::endl;

	std::vector<uint> dfs = grp.dfs();

	for (uint elem: dfs)
	std::cout << elem + 1<< ", ";
	std::cout << std::endl;

	grp.copyInvertedEdges(2);

	// for(const auto& val : topo)
	//     std::cout << val + 1 << ", ";
	// std::cout << std::endl;
	// 
	// 
	// grp.AddEdge(elem2, elem1,   2);
	// grp.AddEdge(elem6, elem1,   2);
	// grp.AddEdge(elem2, elem6,   2);
	// grp.AddEdge(elem7, elem6,   2);
	// grp.AddEdge(elem1, elem5,   2);
	// grp.AddEdge(elem6, elem5,   2);
	// grp.AddEdge(elem11,elem5,   2);
	// grp.AddEdge(elem7, elem6,   2);
	// grp.AddEdge(elem12,elem11,  2);
	// grp.AddEdge(elem3, elem7,   2);
	// grp.AddEdge(elem12,elem7,   2);
	// grp.AddEdge(elem4, elem3,   2);
	// grp.AddEdge(elem8, elem3,   2);
	// grp.AddEdge(elem12,elem13,  2);
	// grp.AddEdge(elem8, elem13,  2);
	// grp.AddEdge(elem14,elem8,   2);
	// grp.AddEdge(elem10,elem4,   2);
	// grp.AddEdge(elem9, elem10,  2);


	std::set<uint> ordersIds;
	std::map<uint, int> ordersIdsWeight;

	/*ordersIds.insert(5);
	ordersIds.insert(7);
	ordersIds.insert(8);
	ordersIds.insert(2);
	while(ordersIds.size() < 4)
	{
	uint r = rand() % 14;
	if (ordersIds.find(r) == ordersIds.end()) {
	ordersIds.insert(rand() % 14);
	ordersIdsWeight[r] = rand() % 10000;
	}
	}

	std::vector<uint> orders;
	orders.resize(ordersIds.size());
	std::copy(ordersIds.begin(), ordersIds.end(), orders.begin());

	// sort special based on index comparison. Complexity = O(K * log(K) * N) K-> number of orders, N -> number of nodes
	std::sort(orders.begin(), orders.end(), [ &topo](uint const& r, uint const& s){
	auto i = std::distance( topo.begin(), std::find(topo.begin(), topo.end(), r)); 
	auto j = std::distance( topo.begin(), std::find(topo.begin(), topo.end(), s)); 
	return i < j;
	});

	for (const auto& elem: orders)
	std::cout << elem + 1<< ", ";
	std::cout << std::endl;

	/* std::map<uint, uint> order = grp.dijkstraShortestPath(src);
	std::vector<uint> path = GetPath(order, src, orders[0]);

	for (size_t i = 1; i < orders.size(); ++i)
	{
	order = grp.dijkstraShortestPath(orders[i-1]);
	std::vector<uint> path1 = GetPath(order, orders[i-1], orders[i]);

	path.insert(path.end(), path1.begin() + 1, path1.end());
	}
	*/

	/*
	uint dijkstraSrc = src;
	std::vector<uint> path;
	path.push_back(src);

	auto srcOrder = std::find(orders.begin(), orders.end(), src);

	if (srcOrder != orders.end())
	orders.erase(srcOrder);

	size_t number_of_times = orders.size();

	for (size_t i = 0; i < number_of_times; ++i) 
	{
	std::map<uint, std::pair<uint, double>> order = grp.dijkstraShortestPath(dijkstraSrc);

	double minDist = std::numeric_limits<double>::infinity();

	for (const auto& elem : order)
	{
	if (elem.first != dijkstraSrc && (std::find(orders.begin(), orders.end(), elem.first) != orders.end()) && (elem.second.second) < minDist)
	minDist = elem.second.second;
	}

	std::vector<uint> minimumDists;

	for (const auto& elem : order)
	{
	if (elem.second.second== minDist && std::find(orders.begin(), orders.end(), elem.first) !=orders.end())
	minimumDists.push_back(elem.first);
	}

	auto nextElem = std::min_element(minimumDists.begin(), minimumDists.end(), [ &orders](uint const& r, uint const& s){
	auto i = std::distance( orders.begin(), std::find(orders.begin(), orders.end(), r)); 
	auto j = std::distance( orders.begin(), std::find(orders.begin(), orders.end(), s)); 
	return i < j;
	});

	std::vector<uint> path1 = GetPath(order, dijkstraSrc, *nextElem);

	path.insert(path.end(), path1.begin() + 1, path1.end());

	if (dijkstraSrc != src)
	orders.erase(std::find(orders.begin(), orders.end(), dijkstraSrc));

	dijkstraSrc = *nextElem;



	}



	/*
	std::map<uint, uint> order = grp.dijkstraShortestPath(src);

	std::pair<uint, uint> minimum = std::make_pair(-1, std::numeric_limits<uint>::max());

	for (uint id : ordersIds)
	{
	if (order[id] < minimum.second)
	{
	minimum.first = id;
	minimum.second = order[id];
	}
	}

	std::vector<uint> path = GetPath(order, src, minimum.first);
	ordersIds.erase(minimum.first);

	while (!ordersIds.empty())
	{
	uint prevMinimum = minimum.first;
	order = grp.dijkstraShortestPath(prevMinimum);

	minimum = std::make_pair(-1, std::numeric_limits<uint>::max());

	for (uint id : ordersIds)
	{
	if (order[id] < minimum.second)
	{
	minimum.first = id;
	minimum.second = order[id];
	}
	}

	std::vector<uint> path1 = GetPath(order, prevMinimum, minimum.first);
	ordersIds.erase(minimum.first);

	path.insert(path.end(), path1.begin() + 1, path1.end());
	}

	//std::cout << std::endl;
	//for (uint elem : orders)
	//	std::cout << elem + 1 << ", ";
	//std::cout << std::endl;

	for (uint elem : path)
	{
	std::cout << elem + 1 << "->";
	}
	std::cout << std::endl;

	*/
	// grp.ShowGraph();


	cin.get();
	return 0;
}



