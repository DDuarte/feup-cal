#include <cute.h>
#include <ide_listener.h>
#include <cute_runner.h>
#include "Person.h"

#include <iostream>
#include <vector>
#include <sstream>

#include "utilities/utilities.hpp"
#include "graph.hpp"

void createNetwork(Graph<Person, double> & net1)
{
    Person p1("Ana",19);
    Person p2("Carlos",33);
    Person p3("Filipe", 20);
    Person p4("Ines", 18);
    Person p5("Maria", 24);
    Person p6("Rui",21);
    Person p7("Vasco",28);
    net1.addVertex(p1); net1.addVertex(p2);
    net1.addVertex(p3); net1.addVertex(p4);
    net1.addVertex(p5); net1.addVertex(p6); net1.addVertex(p7);
    net1.addEdge(p1,p2,0);
    net1.addEdge(p1,p3,0);
    net1.addEdge(p1,p4,0);
    net1.addEdge(p2,p5,0);
    net1.addEdge(p5,p6,0);
    net1.addEdge(p5,p1,0);
    net1.addEdge(p3,p6,0);
    net1.addEdge(p3,p7,0);
    net1.addEdge(p6,p2,0);
}

void test_addVertex()
{
    Graph<Person, double> net1;
    Person p1("Ana",19);
    Person p2("Carlos",33);
    Person p3("Filipe", 20);
    Person p4("Inês", 18);
    net1.addVertex(p1); net1.addVertex(p2);
    net1.addVertex(p3); net1.addVertex(p4);
    ASSERT_EQUAL(false, net1.addVertex(p2));
    ASSERT_EQUAL(4, net1.getNumVertex());
}

void test_removeVertex()
{
    Graph<Person, double> net1;
    Person p1("Ana",19);
    Person p2("Carlos",33);
    Person p3("Filipe", 20);
    Person p4("Inês", 18);
    net1.addVertex(p1); net1.addVertex(p2);
    net1.addVertex(p3); net1.addVertex(p4);
    ASSERT_EQUAL(true, net1.removeVertex(p2));
    ASSERT_EQUAL(false, net1.removeVertex(p2));
    ASSERT_EQUAL(3, net1.getNumVertex());
}

void test_addEdge()
{
    Graph<Person, double> net1;
    Person p1("Ana",19);
    Person p2("Carlos",33);
    Person p3("Filipe", 20);
    Person p4("Inês", 18);
    Person p5("Maria", 24);
    net1.addVertex(p1); net1.addVertex(p2);
    net1.addVertex(p3); net1.addVertex(p4);
    ASSERT_EQUAL(true, net1.addEdge(p1,p2,0));
    ASSERT_EQUAL(true, net1.addEdge(p1,p3,0));
    ASSERT_EQUAL(true, net1.addEdge(p1,p4,0));
    ASSERT_EQUAL(false, net1.addEdge(p2,p5,0));
}

void test_removeEdge()
{
    Graph<Person, double> net1;
    Person p1("Ana",19);
    Person p2("Carlos",33);
    Person p3("Filipe", 20);
    Person p4("Inês", 18);
    Person p5("Maria", 24);
    net1.addVertex(p1); net1.addVertex(p2);
    net1.addVertex(p3); net1.addVertex(p4);
    ASSERT_EQUAL(true, net1.addEdge(p1,p2,0));
    ASSERT_EQUAL(true, net1.addEdge(p1,p3,0));
    ASSERT_EQUAL(true, net1.addEdge(p1,p4,0));
    ASSERT_EQUAL(true, net1.addEdge(p2,p4,0));
    ASSERT_EQUAL(true, net1.removeEdge(p1,p3));
    ASSERT_EQUAL(false, net1.removeEdge(p1,p5));
    ASSERT_EQUAL(false, net1.removeEdge(p2,p3));
}

void test_dfs()
{
    Graph<Person, double> net1;
    createNetwork(net1);
    std::vector<Person> v1=net1.dfs();
    ASSERT_EQUAL("Ana", v1[0].getName());
    ASSERT_EQUAL("Carlos", v1[1].getName());
    ASSERT_EQUAL("Maria", v1[2].getName());
    ASSERT_EQUAL("Rui", v1[3].getName());
    ASSERT_EQUAL("Filipe", v1[4].getName());
    ASSERT_EQUAL("Vasco", v1[5].getName());
    ASSERT_EQUAL("Ines", v1[6].getName());
}

void test_bfs()
{
    Graph<Person, double> net1;
    createNetwork(net1);
    std::vector<Person> v1=net1.bfs(net1.getVertices()[0]);
    ASSERT_EQUAL("Ana", v1[0].getName());
    ASSERT_EQUAL("Carlos", v1[1].getName());
    ASSERT_EQUAL("Filipe", v1[2].getName());
    ASSERT_EQUAL("Ines", v1[3].getName());
    ASSERT_EQUAL("Maria", v1[4].getName());
    ASSERT_EQUAL("Rui", v1[5].getName());
    ASSERT_EQUAL("Vasco", v1[6].getName());
}

void test_removeVertex_Again()
{
    Graph<Person, double> net1;
    createNetwork(net1);
    Person p2("Carlos",33);
    ASSERT_EQUAL(true, net1.removeVertex(p2));
    std::vector<Person> v1=net1.dfs();
    ASSERT_EQUAL("Ana", v1[0].getName());
    ASSERT_EQUAL("Filipe", v1[1].getName());
    ASSERT_EQUAL("Rui", v1[2].getName());
    ASSERT_EQUAL("Vasco", v1[3].getName());
    ASSERT_EQUAL("Ines", v1[4].getName());
    ASSERT_EQUAL("Maria", v1[5].getName());
}

void test_removeEdge_Again()
{
    Graph<Person, double> net1;
    createNetwork(net1);
    Person p5("Maria", 24);
    Person p6("Rui",21);
    ASSERT_EQUAL(true, net1.removeEdge(p5,p6));
    std::vector<Person> v1=net1.dfs();
    ASSERT_EQUAL("Ana", v1[0].getName());
    ASSERT_EQUAL("Carlos", v1[1].getName());
    ASSERT_EQUAL("Maria", v1[2].getName());
    ASSERT_EQUAL("Filipe", v1[3].getName());
    ASSERT_EQUAL("Rui", v1[4].getName());
    ASSERT_EQUAL("Vasco", v1[5].getName());
    ASSERT_EQUAL("Ines", v1[6].getName());
}

void test_maxNumChildren()
{
    Graph<Person, double> net1;
    Person p1("Ana",19);
    Person p2("Carlos",33);
    Person p3("Filipe", 20);
    Person p4("Inês", 18);
    Person p5("Maria", 24);
    Person p6("Rui",21);
    Person p7("Vasco",28);
    net1.addVertex(p1); net1.addVertex(p2);
    net1.addVertex(p3); net1.addVertex(p4);
    net1.addVertex(p5); net1.addVertex(p6); net1.addVertex(p7);
    net1.addEdge(p1,p2,0);
    net1.addEdge(p1,p3,0);
    net1.addEdge(p2,p5,0);
    net1.addEdge(p3,p4,0);
    net1.addEdge(p5,p6,0);
    net1.addEdge(p5,p1,0);
    net1.addEdge(p3,p6,0);
    net1.addEdge(p3,p7,0);
    net1.addEdge(p3,p2,0);
    Person pt;
    ASSERT_EQUAL(4, net1.maxNumChildren(net1.getVertices()[0],pt));
    ASSERT_EQUAL("Filipe", pt.getName());
}

void test_rangeTest()
{
    int arr[11] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::vector<int> vec(arr, arr+11);

    int i = 0;
    for (const auto e : utils::make_range(vec.begin(), vec.end()))
    {
        ASSERT_EQUAL(vec[i], e);
        i++;
    }

    i = vec.size() - 1;
    for (const auto e : utils::reverse(vec))
    {
        ASSERT_EQUAL(vec[i], e);
        i--;
    }
}

void test_a_indegree() {
    Graph<int, double> myGraph;

    myGraph.addVertex(1); myGraph.addVertex(2); myGraph.addVertex(3); myGraph.addVertex(4);

    std::vector<int> vertices = myGraph.getVertices();

    myGraph.addEdge(1, 2, 0);
    myGraph.addEdge(1, 3, 0);
    myGraph.addEdge(4, 2, 0);
    myGraph.addEdge(4, 3, 0);
    myGraph.addEdge(2, 3, 0);

    std::stringstream ss;
    for (unsigned int i = 0; i < vertices.size(); i++){
        ss << vertices[i] << "(" << myGraph.getIndegree(vertices[i]) << ") ";
    }
    ASSERT_EQUAL("1(0) 2(2) 3(3) 4(0) ", ss.str());

    myGraph.removeEdge(4, 3);
    myGraph.addEdge(1, 4, 0);

    ss.str("");
    for (unsigned int i = 0; i < vertices.size(); i++){
        ss << vertices[i] << "(" << myGraph.getIndegree(vertices[i]) << ") ";
    }
    ASSERT_EQUAL("1(0) 2(2) 3(2) 4(1) ", ss.str());

    myGraph.removeVertex(2);
    vertices = myGraph.getVertices();

    ss.str("");
    for (unsigned int i = 0; i < vertices.size(); i++){
        ss << vertices[i] << "(" << myGraph.getIndegree(vertices[i]) << ") ";
    }
    ASSERT_EQUAL("1(0) 3(1) 4(1) ", ss.str());
}

void test_b_getSources() {
    Graph<int, double> myGraph;
    myGraph.addVertex(1); myGraph.addVertex(2); myGraph.addVertex(3); myGraph.addVertex(4);
    std::vector<int> vertices = myGraph.getVertices();
    myGraph.addEdge(1, 2, 0);
    myGraph.addEdge(1, 3, 0);
    myGraph.addEdge(4, 2, 0);
    myGraph.addEdge(4, 3, 0);
    myGraph.addEdge(2, 3, 0);
    myGraph.addEdge(2, 1, 0);

    std::stringstream ss;
    std::vector<int> sources = myGraph.getSources();
    ss << "Num of sources: " << sources.size() << " - ";

    for (unsigned int i = 0; i < sources.size(); i++){
        ss << sources[i] << "(" << myGraph.getIndegree(sources[i]) << ") ";
    }
    ASSERT_EQUAL("Num of sources: 1 - 4(0) ", ss.str());

    myGraph.removeEdge(2, 1);
    ss.str("");
    sources = myGraph.getSources();
    ss << "Num of sources: " << sources.size() << " - ";

    for (unsigned int i = 0; i < sources.size(); i++){
        ss << sources[i] << "(" << myGraph.getIndegree(sources[i]) << ") ";
    }
    ASSERT_EQUAL("Num of sources: 2 - 1(0) 4(0) ", ss.str());
}

void test_c_isDAG() {
    Graph<int, double> myGraph;
    myGraph.addVertex(1); myGraph.addVertex(2); myGraph.addVertex(3); myGraph.addVertex(4); myGraph.addVertex(5);
    myGraph.addEdge(1, 2, 0);
    myGraph.addEdge(2, 5, 0);
    myGraph.addEdge(5, 4, 0);
    myGraph.addEdge(4, 1, 0);
    myGraph.addEdge(5, 1, 0);
    myGraph.addEdge(2, 3, 0);
    myGraph.addEdge(3, 1, 0);

    ASSERT_EQUAL(false, myGraph.isDAG());

    myGraph.removeEdge(4, 1);//
    myGraph.removeEdge(5, 1);//
    myGraph.removeEdge(2, 3);//
    //myGraph.removeEdge(3, 1);//

    ASSERT_EQUAL(true, myGraph.isDAG());

    myGraph.addEdge(1, 4, 0);

    ASSERT_EQUAL(true, myGraph.isDAG());
}

void test_d_topologicaOrder() {
    Graph<int, double> myGraph;
    myGraph.addVertex(1); myGraph.addVertex(2); myGraph.addVertex(3); myGraph.addVertex(4);
    myGraph.addVertex(5); myGraph.addVertex(6); myGraph.addVertex(7);

    myGraph.addEdge(1, 2, 0);
    myGraph.addEdge(1, 4, 0);
    myGraph.addEdge(1, 3, 0);
    myGraph.addEdge(2, 5, 0);
    myGraph.addEdge(2, 4, 0);
    myGraph.addEdge(3, 6, 0);
    myGraph.addEdge(4, 3, 0);
    myGraph.addEdge(4, 6, 0);
    myGraph.addEdge(4, 7, 0);
    myGraph.addEdge(5, 4, 0);
    myGraph.addEdge(5, 7, 0);
    myGraph.addEdge(7, 6, 0);

    std::vector<int> topOrder;

    topOrder = myGraph.topologicalOrder();
    std::stringstream ss;
    for( unsigned int i = 0; i < topOrder.size(); i++) ss << topOrder[i] << " ";
    ASSERT_EQUAL("1 2 5 4 3 7 6 ", ss.str());

    //para testar a chamada do metodo resetIndegrees!
    std::vector<int> vertices = myGraph.getVertices();
    ss.str("");
    for (unsigned int i = 0; i < vertices.size(); i++){
        ss << vertices[i] << "(" << myGraph.getIndegree(vertices[i]) << ") ";
    }
    ASSERT_EQUAL("1(0) 2(1) 3(2) 4(3) 5(1) 6(3) 7(2) ", ss.str());

    //para testar a inclusao de um ciclo no grafo!
    myGraph.addEdge(3, 1, 0);

    topOrder = myGraph.topologicalOrder();
    ss.str("");
    for( unsigned int i = 0; i < topOrder.size(); i++) ss << topOrder[i] << " ";
    ASSERT_EQUAL("", ss.str());
}

void test_e_shortestPath() {
    Graph<int,double> myGraph;
    myGraph.addVertex(1); myGraph.addVertex(2); myGraph.addVertex(3); myGraph.addVertex(4);
    myGraph.addVertex(5); myGraph.addVertex(6); myGraph.addVertex(7);

    myGraph.addEdge(1, 2, 0);
    myGraph.addEdge(1, 4, 0);
    myGraph.addEdge(2, 4, 0);
    myGraph.addEdge(2, 5, 0);
    myGraph.addEdge(3, 1, 0);
    myGraph.addEdge(3, 6, 0);
    myGraph.addEdge(4, 3, 0);
    myGraph.addEdge(4, 5, 0);
    myGraph.addEdge(4, 6, 0);
    myGraph.addEdge(4, 7, 0);
    myGraph.addEdge(5, 7, 0);
    myGraph.addEdge(7, 6, 0);

    //para testar o metodo unweightedShortestPath
    std::vector<int> path = myGraph.getPath(3, 6);

    std::vector<int> vs = myGraph.getVertices();

    std::stringstream ss;
    for(unsigned int i = 0; i < vs.size(); i++) {
        ss << vs[i] << "<-";
        int path;
        if (myGraph.getVertexPath(vs[i], path))  ss << path;
        ss << "|";
    }
    ASSERT_EQUAL("1<-3|2<-1|3<-|4<-1|5<-2|6<-3|7<-4|", ss.str());

    path = myGraph.getPath(3, 7);
    ss.str("");
    for(unsigned int i = 0; i < path.size(); i++) {
        ss << path[i] << " ";
    }
    ASSERT_EQUAL("3 1 4 7 ", ss.str());

    path = myGraph.getPath(5, 6);
    ss.str("");
    for(unsigned int i = 0; i < path.size(); i++) {
        ss << path[i] << " ";
    }
    ASSERT_EQUAL("5 7 6 ", ss.str());

}

void runSuite(){
    cute::suite s;
    //TODO add your test here
    s.push_back(CUTE(test_rangeTest));
    s.push_back(CUTE(test_addVertex));
    s.push_back(CUTE(test_removeVertex));
    s.push_back(CUTE(test_addEdge));
    s.push_back(CUTE(test_removeEdge));
    s.push_back(CUTE(test_dfs));
    s.push_back(CUTE(test_bfs));
    s.push_back(CUTE(test_removeVertex_Again));
    s.push_back(CUTE(test_removeEdge_Again));
    s.push_back(CUTE(test_maxNumChildren));
    s.push_back(CUTE(test_a_indegree));
    s.push_back(CUTE(test_b_getSources));
    s.push_back(CUTE(test_c_isDAG));
    s.push_back(CUTE(test_d_topologicaOrder));
    s.push_back(CUTE(test_e_shortestPath));

    cute::ide_listener lis;
    cute::makeRunner(lis)(s, "CAL 2012/2013 - Aula Pratica 5");
}

int main(){
    runSuite();
    std::cin.get();
    return 0;
}



