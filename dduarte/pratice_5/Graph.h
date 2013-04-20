/*
 * Graph.h
 */
#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>
#include <queue>
using namespace std;

template <class T> class Edge;
template <class T> class Graph;

template <class T>
class Vertex {
    int _indegree;
    T info;
    vector<Edge<T>  > adj;
    bool visited;
    void addEdge(Vertex<T> *dest, double w);
    bool removeEdgeTo(Vertex<T> *d);
public:
    Vertex(T in);
    friend class Graph<T>;
    int getIndegree() const { return _indegree; }
    const T& getInfo() const { return info; }
};

template <class T>
bool Vertex<T>::removeEdgeTo(Vertex<T> *d) {
    typename vector<Edge<T> >::iterator it= adj.begin();
    typename vector<Edge<T> >::iterator ite= adj.end();
    while (it!=ite) {
        if (it->dest == d) {
            --it->dest->_indegree;
            adj.erase(it);
            return true;
        }
        else it++;
    }
    return false;
}

template <class T>
Vertex<T>::Vertex(T in): info(in), visited(false), _indegree(0) {}


template <class T>
void Vertex<T>::addEdge(Vertex<T> *dest, double w) {
    Edge<T> edgeD(dest,w);
    adj.push_back(edgeD);
    ++dest->_indegree;
}

template <class T>
class Edge {
    Vertex<T> * dest;
    double weight;
public:
    Edge(Vertex<T> *d, double w);
    friend class Graph<T>;
    friend class Vertex<T>;
};

template <class T>
Edge<T>::Edge(Vertex<T> *d, double w): dest(d), weight(w){}

template <class T>
class Graph {
    vector<Vertex<T> *> vertexSet;
    void dfs(Vertex<T> *v, vector<T> &res) const;
public:
    bool addVertex(const T &in);
    bool addEdge(const T &sourc, const T &dest, double w);
    bool removeVertex(const T &in);
    bool removeEdge(const T &sourc, const T &dest);
    vector<T> dfs() const;
    vector<T> bfs(Vertex<T> *v) const;
    int maxNewChildren(Vertex<T> *v, T &inf) const;
    vector<Vertex<T> * > getVertexSet() const;
    int getNumVertex() const;

    vector<Vertex<T>*> getSources() const;
    void resetIndegrees();
    bool isDAG();
    vector<T> topologicalOrder();

    bool decentTopologicalOrder(vector<Vertex<T>*>& vec);
};

template <class T>
vector<T> Graph<T>::topologicalOrder()
{
    vector<Vertex<T>*> vec;
    if (!decentTopologicalOrder(vec))
        return vector<T>();

    vector<T> result;
    result.reserve(vec.size());

    for (Vertex<T>* v : vec)
        result.push_back(v->getInfo());

    return result;
}

template <class T>
bool Graph<T>::decentTopologicalOrder(vector<Vertex<T>*>& vec)
{
    std::vector<Vertex<T>*> L; // Empty list that will contain the sorted elements
    std::queue<Vertex<T>*> S; // Set of all nodes with no incoming edges

    resetIndegrees();

    for (auto x : getSources())
        S.push(x);

    int initial_count = vertexSet.size();

    while (!S.empty()) // while S is non-empty do
    {
        Vertex<T>* n = S.front();
        S.pop();
        L.push_back(n);
        initial_count--;

        for (Edge<T> e : n->adj)
        {
            e.dest->_indegree--;
            if (e.dest->_indegree == 0)
                S.push(e.dest);
        }
    }

    resetIndegrees();

    if (L.size() == vertexSet.size()) {
        vec = std::move(L);
        return true;
    }
    else
        return false;
}

template <class T>
void Graph<T>::resetIndegrees()
{
    std::for_each(vertexSet.begin(), vertexSet.end(), [](Vertex<T>* v) { v->_indegree = 0; });

    for (Vertex<T>* v : vertexSet)
    {
        for (Edge<T> e : v->adj)
            e.dest->_indegree++;
    }
}

template <class T>
bool Graph<T>::isDAG()
{
    vector<Vertex<T>*> para_nada;
    return !decentTopologicalOrder(para_nada);
}

template <class T>
vector<Vertex<T>*> Graph<T>::getSources() const
{
    std::vector<Vertex<T>*> result;
    for (const auto& v : vertexSet)
        if (v->getIndegree() == 0)
            result.push_back(v);
    return result;
}

template <class T>
int Graph<T>::getNumVertex() const {
    return vertexSet.size();
}
template <class T>
vector<Vertex<T> * > Graph<T>::getVertexSet() const {
    return vertexSet;
}

template <class T>
bool Graph<T>::addVertex(const T &in) {
    typename vector<Vertex<T>*>::iterator it= vertexSet.begin();
    typename vector<Vertex<T>*>::iterator ite= vertexSet.end();
    for (; it!=ite; it++)
        if ((*it)->info == in) return false;
    Vertex<T> *v1 = new Vertex<T>(in);
    vertexSet.push_back(v1);
    return true;
}

template <class T>
bool Graph<T>::removeVertex(const T &in) {
    typename vector<Vertex<T>*>::iterator it= vertexSet.begin();
    typename vector<Vertex<T>*>::iterator ite= vertexSet.end();
    for (; it!=ite; it++) {
        if ((*it)->info == in) {
            Vertex<T> * v= *it;
            vertexSet.erase(it);

            for (auto it1 = v->adj.begin(); it1 != v->adj.end(); ++it1)
                --it1->dest->_indegree;

            for (auto it1 = vertexSet.begin(); it1 != vertexSet.end(); it1++) {
                (*it1)->removeEdgeTo(v);
            }
            delete v;
            return true;
        }
    }
    return false;
}

template <class T>
bool Graph<T>::addEdge(const T &sourc, const T &dest, double w) {
    typename vector<Vertex<T>*>::iterator it= vertexSet.begin();
    typename vector<Vertex<T>*>::iterator ite= vertexSet.end();
    int found=0;
    Vertex<T> *edgeS, *edgeD;
    while (found!=2 && it!=ite ) {
        if ( (*it)->info == sourc )
            { edgeS=*it; found++;}
        if ( (*it)->info == dest )
            { edgeD=*it; found++;}
        it ++;
    }
    if (found!=2) return false;
    edgeS->addEdge(edgeD,w);
    return true;
}

template <class T>
bool Graph<T>::removeEdge(const T &sourc, const T &dest) {
    typename vector<Vertex<T>*>::iterator it= vertexSet.begin();
    typename vector<Vertex<T>*>::iterator ite= vertexSet.end();
    int found=0;
    Vertex<T> *vS, *vD;
    while (found!=2 && it!=ite ) {
        if ( (*it)->info == sourc )
            { vS=*it; found++;}
        if ( (*it)->info == dest )
            { vD=*it; found++;}
        it ++;
    }
    if (found!=2) return false;
    return vS->removeEdgeTo(vD);
}




template <class T>
vector<T> Graph<T>::dfs() const {
    typename vector<Vertex<T>*>::const_iterator it= vertexSet.begin();
    typename vector<Vertex<T>*>::const_iterator ite= vertexSet.end();
    for (; it !=ite; it++)
        (*it)->visited=false;
    vector<T> res;
    it=vertexSet.begin();
    for (; it !=ite; it++)
        if ( (*it)->visited==false )
            dfs(*it,res);
    return res;
}

template <class T>
void Graph<T>::dfs(Vertex<T> *v,vector<T> &res) const {
    v->visited = true;
    res.push_back(v->info);
    typename vector<Edge<T> >::iterator it= (v->adj).begin();
    typename vector<Edge<T> >::iterator ite= (v->adj).end();
    for (; it !=ite; it++)
        if ( it->dest->visited == false )
            dfs(it->dest, res);
}

template <class T>
vector<T> Graph<T>::bfs(Vertex<T> *v) const {
    vector<T> res;
    queue<Vertex<T> *> q;
    q.push(v);
    v->visited = true;
    while (!q.empty()) {
        Vertex<T> *v1 = q.front();
        q.pop();
        res.push_back(v1->info);
        typename vector<Edge<T> >::iterator it=v1->adj.begin();
        typename vector<Edge<T> >::iterator ite=v1->adj.end();
        for (; it!=ite; it++) {
            Vertex<T> *d = it->dest;
            if (d->visited==false) {
                d->visited=true;
                q.push(d);
            }
        }
    }
    return res;
}

template <class T>
int Graph<T>::maxNewChildren(Vertex<T> *v, T &inf) const {
    vector<T> res;
    queue<Vertex<T> *> q;
    queue<int> level;
    int maxChildren=0;
    inf =v->info;
    q.push(v);
    level.push(0);
    v->visited = true;
    while (!q.empty()) {
        Vertex<T> *v1 = q.front();
        q.pop();
        res.push_back(v1->info);
        int l=level.front();
        level.pop(); l++;
        int nChildren=0;
        typename vector<Edge<T> >::iterator it=v1->adj.begin();
        typename vector<Edge<T> >::iterator ite=v1->adj.end();
        for (; it!=ite; it++) {
            Vertex<T> *d = it->dest;
            if (d->visited==false) {
                d->visited=true;
                q.push(d);
                level.push(l);
                nChildren++;
            }
        }
        if (nChildren>maxChildren) {
            maxChildren=nChildren;
            inf = v1->info;
        }
    }
    return maxChildren;
}



#endif /* GRAPH_H_ */
