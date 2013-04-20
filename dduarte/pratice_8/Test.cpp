#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "Graph.h"
#include <iostream>
#include <string>
using namespace std;

Graph<int> createTestGraph()
{
    Graph<int> myGraph;

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
    myGraph.addEdge(4, 7, 4);
    myGraph.addEdge(5, 7, 2);
    myGraph.addEdge(6, 4, 3);
    myGraph.addEdge(7, 6, 4);

    // to force undirected graph...
    myGraph.addEdge(2, 1, 2);
    myGraph.addEdge(4, 1, 7);
    myGraph.addEdge(4, 2, 3);
    myGraph.addEdge(5, 2, 5);
    myGraph.addEdge(1, 3, 2);
    myGraph.addEdge(6, 3, 5);
    myGraph.addEdge(3, 4, 1);
    myGraph.addEdge(5, 4, 1);
    myGraph.addEdge(7, 4, 4);
    myGraph.addEdge(7, 5, 2);
    myGraph.addEdge(4, 6, 3);
    myGraph.addEdge(6, 7, 4);
    return myGraph;
}

Graph<int> createTestFlowGraph()
{
    Graph<int> myGraph;

    for(int i = 1; i < 7; i++)
        myGraph.addVertex(i);

    myGraph.addEdge(1, 2, 0, 3);
    myGraph.addEdge(1, 3, 0, 2);
    myGraph.addEdge(2, 5, 0, 4);
    myGraph.addEdge(2, 4, 0, 3);
    myGraph.addEdge(2, 3, 0, 1);
    myGraph.addEdge(3, 5, 0, 2);
    myGraph.addEdge(4, 6, 0, 2);
    myGraph.addEdge(5, 6, 0, 3);

    return myGraph;
}


void testPrim() {
    Graph<int> graph = createTestGraph();
    vector<Vertex<int>* > res = graph.calculatePrim();

    stringstream ss;
    for(unsigned int i = 0; i < res.size(); i++)
    {
        ss << res[i]->getInfo() << "<-";
        cout << res[i]->getInfo() << "<-";

        if ( res[i]->getPath() != NULL )
        {
            Vertex<int>* v = res[i]->getPath();
            ss << v->getInfo();
            cout << v->getInfo();
        }

        ss << "|";
        cout << "|";
    }

    cout << endl << endl;

//    ASSERT_EQUAL("1<-|2<-1|3<-4|4<-2|5<-4|6<-7|7<-5|", ss.str());
    ASSERT_EQUAL("1<-|2<-1|3<-1|4<-3|5<-4|6<-4|7<-5|", ss.str());
}

void testKruskal() {
/*    Graph<int> graph = createTestGraph();
    vector<Vertex<int>* > res = graph.calculateKruskal();

    stringstream ss;
    for(unsigned int i = 0; i < res.size(); i++)
    {
        ss << res[i]->getInfo() << "<-";
        cout << res[i]->getInfo() << "<-";

        if ( res[i]->getPath() != NULL )
        {
            Vertex<int>* v = res[i]->getPath();
            ss << v->getInfo();
            cout << v->getInfo();
        }

        ss << "|";
        cout << "|";
    }

    cout << endl << endl;

//    ASSERT_EQUAL("1<-3|2<-1|3<-4|4<-6|5<-4|6<-|7<-5|", ss.str());
    ASSERT_EQUAL("1<-3|2<-1|3<-4|4<-5|5<-7|6<-4|7<-|", ss.str());*/
}

void testFordFulkerson()
{
    /*Graph<int> graph = createTestFlowGraph();
    vector<Vertex<int>* > res = graph.calculateFordFulkerson(1);

    stringstream ss;
    for(unsigned int i = 0; i < res.size(); i++)
    {
        ss << res[i]->getInfo() << "-> (";
        cout << res[i]->getInfo() << "-> (";

        vector<Edge<int> > edges = res[i]->getAdj();
        for (unsigned int a = 0; a < edges.size(); a++)
        {
            ss << (edges[a].getDest())->getInfo() << "[Flow: " << edges[a].getFlow() << "] ";
            cout << (edges[a].getDest())->getInfo() << "[Flow: " << edges[a].getFlow() << "] ";
        }

        ss << ") || ";
        cout << ") || ";
    }

    cout << endl << endl;

    ASSERT_EQUAL("1-> (2[Flow: 3] 3[Flow: 2] ) || 2-> (5[Flow: 1] 4[Flow: 2] 3[Flow: 0] ) || 3-> (5[Flow: 2] ) || 4-> (6[Flow: 2] ) || 5-> (6[Flow: 3] ) || 6-> () || ", ss.str());*/
}

void runSuite(){
    cute::suite s;
    //TODO add your test here
    s.push_back(CUTE(testPrim));
    s.push_back(CUTE(testKruskal));
    s.push_back(CUTE(testFordFulkerson));
    cute::ide_listener lis;
    cute::makeRunner(lis)(s, "The Suite");
}

class A
{
private:
    int a;

public:
    class B
    {
        void b() { A a; a.a; }
    };
};

int main()
{
    runSuite();

    system("PAUSE");
    return 0;
}
