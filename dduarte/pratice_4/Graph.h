/*
 * Graph.h
 */
#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>
#include <algorithm>
using namespace std;

template <class T> class Edge;
template <class T> class Graph;

template <class T>
class Vertex {
	T info;
	vector<Edge<T>> adj;
	bool visited;
public:
	Vertex(T in);
	friend class Graph<T>;
};

template <class T>
class Edge {
	Vertex<T>* dest;
	double weight;
public:
	Edge(Vertex<T>* d, double w);
	friend class Graph<T>;
	friend class Vertex<T>;
};

template <class T>
class Graph {
	vector<Vertex<T>*> vertexSet;
public:
	vector<Vertex<T>*> getVertexSet() const;
	int getNumVertex() const;
    bool addVertex(const T& in);
    bool addEdge(const T& sourc, const T& dest, double w);
    bool removeVertex(const T& in);
    bool removeEdge(const T& sourc, const T& dest);

    vector<T> dfs() const;

private:
    void dfs(Vertex<T>* v, vector<T>& vec) const;
};

template<class T>
vector<T> Graph<T>::dfs() const
{
    vector<T> result;

    for (Vertex<T>* v : vertexSet)
        v->visited = false;

    for (Vertex<T>* v : vertexSet)
    {
        if (!v->visited)
        {
            result.push_back(v->info);
            dfs(v, result);
        }
    }

    return result;
}

template<class T>
void Graph<T>::dfs(Vertex<T>* v, vector<T>& vec) const
{
    v->visited = true;

    for (const auto& e : v->adj)
    {
        if (!e.dest->visited)
        {
            vec.push_back(e.dest->info);
            dfs(e.dest, vec);
            
        }
    }
}

template <class T>
bool Graph<T>::removeEdge(const T& sourc, const T& dest)
{
    for (Vertex<T>* v : vertexSet)
    {
        if (v->info == sourc)
        {
            for (auto it = v->adj.begin(); it != v->adj.end(); ++it)
            {
                if (it->dest->info == dest)
                {
                    v->adj.erase(it);
                    return true;
                }
            }
        }
    }

    return false;
}

template <class T>
bool Graph<T>::removeVertex(const T& in)
{
    for (auto it = vertexSet.begin(); it != vertexSet.end(); ++it)
    {
        for (auto it2 = (*it)->adj.begin(); it2 != (*it)->adj.end(); ++it2)
        {
            if (it2->dest->info == in)
                it2 = (*it)->adj.erase(it2);
        }
    }

    for (auto it = vertexSet.begin(); it != vertexSet.end(); ++it)
    {
        if ((*it)->info == in)
        {
            delete (*it);
            vertexSet.erase(it);
            return true;
        }
    }

    return false;
}

template <class T>
bool Graph<T>::addEdge(const T& sourc, const T& dest, double w)
{
    for (Vertex<T>* v : vertexSet)
    {
        if (v->info == sourc)
        {
            for (Vertex<T>* vv : vertexSet)
            {
                if (vv->info == dest)
                {
                    v->adj.push_back(Edge<T>(vv, w));
                    return true;
                }
            }
        }
    }

    return false;
}

template <class T>
bool Graph<T>::addVertex(const T& in)
{
    for (Vertex<T>* v : vertexSet)
        if (v->info == in)
            return false;

    vertexSet.push_back(new Vertex<T>(in));
    return true;
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
Edge<T>::Edge(Vertex<T> *d, double w): dest(d), weight(w){}

template <class T>
Vertex<T>::Vertex(T in): info(in), visited(false){}

#endif /* GRAPH_H_ */
