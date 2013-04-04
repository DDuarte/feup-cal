#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include "utilities/utilities.hpp"

#include <vector>
#include <stack>
#include <queue>
#include <algorithm>

template <class V, class E>
class Graph
{
private:
    struct Edge;

    struct Vertex
    {
        Vertex(const V& in);
        V info;
        std::vector<Edge> adj;
        mutable bool visited;
        int indegree;
        int dist;
        Vertex* path;
    };

    struct Edge
    {
        Edge(Vertex* d, const E& w);
        Vertex* dest;
        E weight;
    };

public:  
    std::vector<V> dfs() const;
    std::vector<V> bfs(const V& in) const;
    
    int getNumVertex() const;
    bool addVertex(const V &in);
    bool removeVertex(const V& in);

    bool addEdge(const V& sourc, const V& dest, const E& weight);
    bool removeEdge(const V& sourc, const V& dest);

    unsigned int maxNumChildren(const V& sourc, V& in) const;
    int getIndegree(const V& in) const;
    std::vector<V> getSources() const;
    bool isDAG();

    std::vector<V> getVertices() const;
    std::vector<V> topologicalOrder();

    void unweightedShortestPath(const V &in) const;
    std::vector<V> getPath( const V &origin, const V& dest) const;
    bool getVertexPath( const V &in, V& out) const;
private:
    void resetIndegree();

    std::vector<Vertex*> _vertices;
};

template <class V, class E>
bool Graph<V, E>::getVertexPath( const V &in, V& out ) const
{
    auto elem = std::find_if(_vertices.begin(), _vertices.end(), [&in] (Vertex* ver) { return ver->info == in; });
    
    if (elem == _vertices.end()) return false;

    if ((*elem)->path == nullptr) return false;

    out = (*elem)->path->info;
    
    return true;
}

template <class V, class E>
void Graph<V, E>::unweightedShortestPath( const V &in ) const
{
    auto elem = std::find_if(_vertices.begin(), _vertices.end(), [&in] (Vertex* ver) { return ver->info == in; });
    if (elem == _vertices.end()) return;

    Vertex* v = (*elem);

    for (Vertex* ver : _vertices)
    {
        ver->path = nullptr;
        ver->dist = std::numeric_limits<int>::infinity();
        ver->visited = false;
    }

    v->dist = 0;

    std::queue<Vertex*> q;
    q.push(v);
    v->visited = true;

    while (!q.empty())
    {
        v = q.front(); q.pop();

        for (Edge& edge : v->adj)
        {
            if (!edge.dest->visited)
            {
                edge.dest->visited = true;
                edge.dest->dist = v->dist + 1;
                edge.dest->path = v;
                q.push(edge.dest);
            }
        }
    }

}

template <class V, class E>
std::vector<V> Graph<V, E>::getPath( const V &origin, const V& dest) const
{
    auto org = std::find_if(_vertices.begin(), _vertices.end(), [&origin] (Vertex* ver) { return ver->info == origin; });
    if (org == _vertices.end()) return std::vector<V>();

    auto dst = std::find_if(_vertices.begin(), _vertices.end(), [&dest] (Vertex* ver) { return ver->info == dest; });
    if (dst == _vertices.end()) return std::vector<V>();

    unweightedShortestPath(origin);

    std::vector<V> result;

    for (Vertex* o = *dst; o->path != nullptr; o = o->path)
    {
        result.push_back(o->info);
        if ( o->path == *org) {
            result.push_back(o->path->info);
            break;
        }
    }

    std::reverse(result.begin(), result.end());

    return result;
}

template <class V, class E>
void Graph<V, E>::resetIndegree()
{
    for (Vertex* ver : _vertices)
        ver->indegree = 0;

    for (Vertex* ver : _vertices)
        for (Edge& edge : ver->adj)
            edge.dest->indegree++;
}

template <class V, class E>
std::vector<V> Graph<V, E>::topologicalOrder()
{
    std::vector<V> result;

    resetIndegree();

    std::queue<Vertex*> C;

    for (Vertex* ver : _vertices)
        if (ver->indegree == 0)
            C.push(ver);

    while (!C.empty())
    {
        Vertex* ver = C.front();
        C.pop();

        result.push_back(ver->info);

        for (Edge& edge : ver->adj)
        {
            edge.dest->indegree--;
            if (edge.dest->indegree == 0)
                C.push(edge.dest);
        }
    }

    if (result.size() != _vertices.size())
        result.erase(result.begin(), result.end());

    resetIndegree();

    return result;
}

template <class V, class E>
int Graph<V, E>::getIndegree( const V& in ) const
{
    auto elem = std::find_if(_vertices.begin(), _vertices.end(), [&in] (Vertex* ver) { return ver->info == in; });
    if (elem == _vertices.end())
        return -1;

    return (*elem)->indegree;
}

template <class V, class E> Graph<V, E>::Vertex::Vertex(const V& in) : info(in), visited(false), indegree(0), dist(0), path(nullptr) { }
template <class V, class E> Graph<V, E>::Edge::Edge(Vertex* d,  const E& w ) :dest(d), weight(w) { }


template <class V, class E> int Graph<V, E>::getNumVertex() const { return _vertices.size(); }

template <class V, class E>
bool Graph<V, E>::addVertex( const V &in )
{
    if (std::find_if(_vertices.begin(), _vertices.end(), [&in] (Vertex* ver) { return in == ver->info; }) != _vertices.end())
        return false;

    _vertices.push_back(new Vertex(in));

    return true;
}

template <class V, class E>
bool Graph<V, E>::removeVertex( const V& in )
{
    auto elem = std::find_if(_vertices.begin(), _vertices.end(), [&in] (Vertex* ver) { return in == ver->info; });

    if (elem == _vertices.end()) return false;

    for (Vertex* ver : _vertices)
    {
        ver->adj.erase(std::remove_if(ver->adj.begin(), ver->adj.end(), 
                                    [&elem] (Edge& edge) 
                                    { 
                                        return edge.dest->info == (*elem)->info;
                                    }), 
                      ver->adj.end());
    }

    for (Edge& edge : (*elem)->adj)
        edge.dest->indegree--;

    delete *elem;

    _vertices.erase(elem);

    return true;
}

template <class V, class E>
bool Graph<V, E>::addEdge(const V& sourc, const V& dest, const E& weight)
{
    auto src = std::find_if(_vertices.begin(), _vertices.end(), [&sourc] (Vertex* ver) { return sourc == ver->info; });

    if (src == _vertices.end()) return false;

    auto dst = std::find_if(_vertices.begin(), _vertices.end(), [&dest] (Vertex* ver) { return dest == ver->info; });

    if (dst == _vertices.end()) return false;

    if (std::find_if((*src)->adj.begin(), (*src)->adj.end(), [&dst] (Edge edge) { return (*dst)->info == edge.dest->info; }) != (*src)->adj.end()) return false;

    (*dst)->indegree++;

    (*src)->adj.push_back(Edge(*dst, weight));

    return true;
}

template <class V, class E>
bool Graph<V, E>::removeEdge( const V& sourc, const V& dest )
{
    auto src = std::find_if(_vertices.begin(), _vertices.end(), [&sourc] (Vertex* ver) { return sourc == ver->info; });

    if (src == _vertices.end()) return false;

    auto dst = std::find_if(_vertices.begin(), _vertices.end(), [&dest] (Vertex* ver) { return dest == ver->info; });

    if (dst == _vertices.end()) return false;

    auto edge = std::find_if((*src)->adj.begin(), (*src)->adj.end(), [&dst] (Edge edge) { return (*dst)->info == edge.dest->info; });

    if (edge == (*src)->adj.end()) return false;

    (*dst)->indegree--;

    (*src)->adj.erase(edge);

    return true;
}

template <class V, class E>
std::vector<V> Graph<V, E>::dfs() const
{
    std::for_each(_vertices.begin(), _vertices.end(), [] (Vertex* ver) { ver->visited = false; });

    std::vector<V> result;

    std::stack<Vertex*> stateStk;

    for (std::vector<Vertex*>::const_iterator v = _vertices.cbegin(); v != _vertices.cend(); v++)
    {
        if ((*v)->visited) continue;

        const Vertex* ver = *v;

        do 
        {
            if (!ver->visited) result.push_back(ver->info);

            ver->visited = true;

            for (const auto& e : utils::reverse(ver->adj)) 
            {
                if (!e.dest->visited)
                {
                    stateStk.push(e.dest);
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

template <class V, class E>
std::vector<V> Graph<V, E>::bfs(const V &in) const
{
    std::vector<Vertex*>::const_iterator elem = std::find_if(_vertices.begin(), _vertices.end(), [&in] (Vertex* ver) { return ver->info == in; } );

    if (elem == _vertices.end()) return std::vector<V>();

    const Vertex* v = *elem;

    std::for_each(_vertices.begin(), _vertices.end(), [] (Vertex* ver) { ver->visited = false; });

    std::queue<const Vertex*> q;
    std::vector<V> result;

    q.push(v);

    v->visited = true;

    while (!q.empty())
    {
        v = q.front();
        q.pop();

        for (Edge e : v->adj)
        {
            const Vertex* w = e.dest;
            if (!w->visited)
            {
                w->visited = true;
                q.push(w);
            }
        }

        result.push_back(v->info);
    }

    return result;
}

template <class V, class E>
unsigned int Graph<V, E>::maxNumChildren( const V& sourc, V& in ) const
{
    std::vector<Vertex*>::const_iterator elem = std::find_if(_vertices.begin(), _vertices.end(), [&sourc] (Vertex* ver) { return ver->info == sourc; } );
    if (elem == _vertices.cend()) return 0;
    const Vertex* v = *elem;

    std::for_each(_vertices.begin(), _vertices.end(), [] (Vertex* ver) { ver->visited = false; });

    std::queue<const Vertex*> q;
    unsigned int numChildren = 0;

    q.push(v);
    v->visited = true;

    while (!q.empty())
    {
        v = q.front();
        q.pop();

        for (Edge e : v->adj)
        {
            const Vertex* w = e.dest;
            if (!w->visited)
            {
                w->visited = true;
                q.push(w);
            }
        }

        if (v->adj.size() > numChildren)
        {
            in = v->info;
            numChildren = v->adj.size();
        }
    }

    return numChildren;
}

template <class V, class E>
std::vector<V> Graph<V, E>::getVertices() const
{
    std::vector<V> result;

    for (const auto& v: _vertices)
        result.push_back(v->info);

    return result;
}



template <class V, class E>
std::vector<V> Graph<V,E>::getSources() const 
{
    std::vector<V> result;

    for (const Vertex* ver : _vertices)
        if (ver->indegree == 0)
            result.push_back(ver->info);

    return result;
}

template <class V, class E>
bool Graph<V,E>:: isDAG() 
{
    bool result = true;
    unsigned int size = 0;

    resetIndegree();

    std::queue<Vertex*> C;

    for (Vertex* ver : _vertices)
        if (ver->indegree == 0)
            C.push(ver);

    while (!C.empty())
    {
        Vertex* ver = C.front();
        C.pop();
        size++;
        for (Edge& edge : ver->adj)
        {
            edge.dest->indegree--;
            if (edge.dest->indegree == 0)
                C.push(edge.dest);
        }
    }

    if (size != _vertices.size())
        result = false;

    resetIndegree();

    return result;
}

#endif // GRAPH_HPP_ 