#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Graph.h"
#include <sstream>

using namespace std;

void test_a_indegree() {
    Graph<int> myGraph;

    myGraph.addVertex(1); myGraph.addVertex(2); myGraph.addVertex(3); myGraph.addVertex(4);

    vector< Vertex<int>* > vertices = myGraph.getVertexSet();

    myGraph.addEdge(1, 2, 0);
    myGraph.addEdge(1, 3, 0);
    myGraph.addEdge(4, 2, 0);
    myGraph.addEdge(4, 3, 0);
    myGraph.addEdge(2, 3, 0);

    stringstream ss;
    for (unsigned int i = 0; i < vertices.size(); i++){
        ss << vertices[i]->getInfo() << "(" << vertices[i]->getIndegree() << ") ";
    }
    ASSERT_EQUAL("1(0) 2(2) 3(3) 4(0) ", ss.str());

    myGraph.removeEdge(4, 3);
    myGraph.addEdge(1, 4, 0);

    ss.str("");
    for (unsigned int i = 0; i < vertices.size(); i++){
        ss << vertices[i]->getInfo() << "(" << vertices[i]->getIndegree() << ") ";
    }
    ASSERT_EQUAL("1(0) 2(2) 3(2) 4(1) ", ss.str());

    myGraph.removeVertex(2);
    vertices = myGraph.getVertexSet();

    ss.str("");
    for (unsigned int i = 0; i < vertices.size(); i++){
        ss << vertices[i]->getInfo() << "(" << vertices[i]->getIndegree() << ") ";
    }
    ASSERT_EQUAL("1(0) 3(1) 4(1) ", ss.str());
}

void test_b_getSources() {
    Graph<int> myGraph;
    myGraph.addVertex(1); myGraph.addVertex(2); myGraph.addVertex(3); myGraph.addVertex(4);
    vector< Vertex<int>* > vertices = myGraph.getVertexSet();
    myGraph.addEdge(1, 2, 0);
    myGraph.addEdge(1, 3, 0);
    myGraph.addEdge(4, 2, 0);
    myGraph.addEdge(4, 3, 0);
    myGraph.addEdge(2, 3, 0);
    myGraph.addEdge(2, 1, 0);

    stringstream ss;
    vector< Vertex<int>* > sources = myGraph.getSources();
    ss << "Num of sources: " << sources.size() << " - ";

    for (unsigned int i = 0; i < sources.size(); i++){
        ss << sources[i]->getInfo() << "(" << sources[i]->getIndegree() << ") ";
    }
    ASSERT_EQUAL("Num of sources: 1 - 4(0) ", ss.str());

    myGraph.removeEdge(2, 1);
    ss.str("");
    sources = myGraph.getSources();
    ss << "Num of sources: " << sources.size() << " - ";

    for (unsigned int i = 0; i < sources.size(); i++){
        ss << sources[i]->getInfo() << "(" << sources[i]->getIndegree() << ") ";
    }
    ASSERT_EQUAL("Num of sources: 2 - 1(0) 4(0) ", ss.str());
}

void test_c_isDAG() {
    Graph<int> myGraph;
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
    Graph<int> myGraph;
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

    vector<int> topOrder;

    topOrder = myGraph.topologicalOrder();
    stringstream ss;
    for( unsigned int i = 0; i < topOrder.size(); i++) ss << topOrder[i] << " ";
    ASSERT_EQUAL("1 2 5 4 3 7 6 ", ss.str());

    //para testar a chamada do metodo resetIndegrees!
    vector< Vertex<int>* > vertices = myGraph.getVertexSet();
    ss.str("");
    for (unsigned int i = 0; i < vertices.size(); i++){
        ss << vertices[i]->getInfo() << "(" << vertices[i]->getIndegree() << ") ";
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
    /*Graph<int> myGraph;
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
    vector<int> path = myGraph.getPath(3, 6);

    vector<Vertex<int>* > vs = myGraph.getVertexSet();

    stringstream ss;
    for(unsigned int i = 0; i < vs.size(); i++) {
        ss << vs[i]->getInfo() << "<-";
        if ( vs[i]->path != NULL )  ss << vs[i]->path->getInfo();
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
*/
}


void runSuite(){
    cute::suite s;
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

    std::cout << "Press ENTER to continue..." << std::endl;
    std::cin.get();
    return 0;
}


