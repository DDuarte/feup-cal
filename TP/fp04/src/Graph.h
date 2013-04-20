/*
 * Graph.h
 */
#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>
#include <stack>
#include <queue>
#include <algorithm>

template<class Fwd>
struct Reverser {
    const Fwd &fwd;
    Reverser<Fwd>(const Fwd &fwd_): fwd(fwd_) {}
    auto begin() -> decltype(fwd.rbegin()) const { return fwd.rbegin(); }
    auto end() -> decltype(fwd.rend()) const  { return fwd.rend(); }
};

template<class Fwd>
Reverser<Fwd> reverse(const Fwd &fwd) { return Reverser<Fwd>(fwd); }

template <class T, class E> class Edge;
template <class T, class E> class Graph;

template <class T, class E>
class Vertex
{
public:
    Vertex(T in);
    friend class Graph<T, E>;
private:
    T _info;
    std::vector<Edge<T, E>> _adj;
    bool _visited;
};

template <class T, class E>
class Edge
{
public:
    Edge(Vertex<T, E> *d, E w);
    friend class Graph<T, E>;
    friend class Vertex<T, E>;
private:
    Vertex<T, E> * _dest;
    E _weight;
};

template <class T, class E>
class Graph
{
public:
    std::vector<Vertex<T, E>*> getVertexSet() const;
    int getNumVertex() const;
    bool addVertex(const T &in);
    std::vector<T> dfs() const;
    std::vector<T> bfs(Vertex<T, E>* v) const;
    bool removeVertex(const T& in);
    bool addEdge(const T &sourc, const T &dest, E w);
    bool removeEdge(const T& sourc, const T& dest);
    int maxNewChildren(Vertex<T, E>* v, T& in) const;
private:
    std::vector<Vertex<T, E>*> _vertexSet;
};

template <class T, class E> Vertex<T, E>::Vertex( T in ) : _info(in), _visited(false) { }
template <class T, class E> Edge<T, E>::Edge( Vertex<T, E> *d, E w ) : _dest(d), _weight(w) { }
template <class T, class E> int Graph<T, E>::getNumVertex() const { return _vertexSet.size(); }

template <class T, class E> std::vector<Vertex<T, E>*> Graph<T, E>::getVertexSet() const
{
    return _vertexSet;
}


template <class T, class E>
bool Graph<T, E>::addVertex(const T &in)
{
    if (std::find_if(_vertexSet.begin(), _vertexSet.end(), [&in] (Vertex<T, E>* ver) { return in == ver->_info; }) != _vertexSet.end())
        return false;

    _vertexSet.push_back(new Vertex<T,E>(in));

    return true;
}

template <class T, class E>
bool Graph<T, E>::removeVertex( const T& in )
{
    auto elem = std::find_if(_vertexSet.begin(), _vertexSet.end(), [&in] (Vertex<T, E>* ver) { return in == ver->_info; });

    if (elem == _vertexSet.end())
        return false;

    for (auto& ver : _vertexSet)
        ver->_adj.erase(std::remove_if(ver->_adj.begin(), ver->_adj.end(), [&elem](Edge<T, E> edge) { return edge._dest == *elem; }), ver->_adj.end());

    delete (*elem);

    _vertexSet.erase(elem);

    return true;
}


template <class T, class E>
bool Graph<T, E>::addEdge(const T &sourc, const T &dest, E w)
{
    auto src = std::find_if(_vertexSet.begin(), _vertexSet.end(), [&sourc] (Vertex<T, E>* ver) { return sourc == ver->_info; });

    if (src == _vertexSet.end())
        return false;

    auto dst = std::find_if(_vertexSet.begin(), _vertexSet.end(), [&dest] (Vertex<T, E>* ver) { return dest == ver->_info; });

    if (dst == _vertexSet.end())
        return false;

    (*src)->_adj.push_back(Edge<T,E>(*dst, w));

    return true;
}


template <class T, class E>
bool Graph<T, E>::removeEdge(const T& sourc, const T& dest)
{
    auto src = std::find_if(_vertexSet.begin(), _vertexSet.end(), [&sourc] (Vertex<T, E>* ver) { return sourc == ver->_info; });

    if (src == _vertexSet.end())
        return false;

    auto dst = std::find_if(_vertexSet.begin(), _vertexSet.end(), [&dest] (Vertex<T, E>* ver) { return dest == ver->_info; });

    if (dst == _vertexSet.end())
        return false;

    auto edge = std::find_if((*src)->_adj.begin(), (*src)->_adj.end(), [&dst] (Edge<T, E> edg) { return edg._dest == (*dst); });

    if (edge == (*src)->_adj.end())
        return false;

    (*src)->_adj.erase(edge);

    return true;
}

template <class T, class E>
std::vector<T> Graph<T, E>::dfs() const
{
    std::for_each(_vertexSet.begin(), _vertexSet.end(), [] (Vertex<T, E>* ver) { ver->_visited = false; });

    std::vector<T> result;

    std::stack<Vertex<T,E>*> stateStk;

    for (size_t i = 0; i < _vertexSet.size(); i++)
    {
        if (_vertexSet[i]->_visited) continue;

        Vertex<T,E>* ver = _vertexSet[i];

        do
        {
            if (! ver->_visited)
                result.push_back(ver->_info);

            ver->_visited = true;

            for (const auto& e: reverse(ver->_adj))
            {
                if (!e._dest->_visited)
                {
                    stateStk.push(e._dest);
                }
            }

            if (!stateStk.empty())
            {
                ver = stateStk.top();
                stateStk.pop();
            }
        } while (!stateStk.empty());
    }

    return result;
}


template <class T, class E>
std::vector<T> Graph<T, E>::bfs(Vertex<T, E>* v) const
{
    std::for_each(_vertexSet.begin(), _vertexSet.end(), [] (Vertex<T, E>* ver) { ver->_visited = false; });

    std::queue<Vertex<T,E>*> q;
    std::vector<T> result;

    q.push(v);
    v->_visited = true;

    while (q.size() > 0)
    {
        v = q.front();
        q.pop();

        for (Edge<T,E> e : v->_adj)
        {
            Vertex<T,E>* w = e._dest;
            if (!w->_visited)
            {
                w->_visited = true;
                q.push(w);
            }
        }

        result.push_back(v->_info);
    }

    return result;
}

template <class T, class E>
int Graph<T, E>::maxNewChildren(Vertex<T, E>* v, T& in) const
{
    std::for_each(_vertexSet.begin(), _vertexSet.end(), [] (Vertex<T, E>* ver) { ver->_visited = false; });

    std::queue<Vertex<T,E>*> q;
    int numChildren = 0;

    q.push(v);
    v->_visited = true;

    while (q.size() > 0)
    {
        v = q.front();
        q.pop();

        for (Edge<T,E> e : v->_adj)
        {
            Vertex<T,E>* w = e._dest;
            if (!w->_visited)
            {
                w->_visited = true;
                q.push(w);
            }
        }

        if (v->_adj.size() > numChildren)
        {
            in = v->_info;
            numChildren = v->_adj.size();
        }
    }

    return numChildren;
}

#endif /* GRAPH_H_ */
