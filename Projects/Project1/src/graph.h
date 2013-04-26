#ifndef GRAPH_H_
#define GRAPH_H_

#include "utils.h"
#include "graphviewer.h"

#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <algorithm>
#include <exception>

#undef max

/**
 * Graph Data Structure
 *  
 */
template <class V, class E>
class Graph
{
public:
    /**
    * Encapsulates information about a Dijkstra shortest path
    */
    struct DijkstraPath
    {
        /**
        * Encapsulates information about a Dijkstra shortest path vertex
        */
        struct DijkstraVertex
        {
            DijkstraVertex(uint p = std::numeric_limits<uint>::max(), double d = std::numeric_limits<double>::infinity()) : PathId(p), Dist(d) { } ///< Constructor
            DijkstraVertex(const DijkstraVertex& other) : PathId(other.PathId), Dist(other.Dist) { } ///< Copy constructor
            DijkstraVertex operator=(const DijkstraVertex& other) { return DijkstraVertex(other); } ///< Assignment operator
            const uint PathId;
            const double Dist;
        };

        std::map<uint, typename Graph<V,E>::DijkstraPath::DijkstraVertex> Vertices;

        typedef typename std::map<uint, DijkstraVertex>::iterator iterator;
        typedef typename std::map<uint, DijkstraVertex>::const_iterator const_iterator;
        typedef typename std::map<uint, DijkstraVertex>::value_type value_type;
        typedef typename std::map<uint, DijkstraVertex>::reference reference;
        typedef typename std::map<uint, DijkstraVertex>::const_reference const_reference;

        iterator begin(); ///< Returns an iterator to the begin of the vertex map. Allows the struct to be iterated.
        iterator end(); ///< Returns an iterator to the end  of the vertex map. Allows the struct to be iterated.
        const_iterator begin() const; ///< Returns a const iterator to the begin of the vertex map. Allows the struct to be iterated.
        const_iterator end() const; ///< Returns a const iterator to the end of the vertex map. Allows the struct to be iterated.

        std::vector<uint> GetPath(uint src, uint dst) const; ///< Returns the path from the specified source to the specified destination
    };

    // Inner Classes Declarations
protected:
    /**
    * Encapsulates information about a graph edge
    */
    struct Edge
    {
        uint idDest;
        E weight;

        Edge(uint destId, const E& whgt) : idDest(destId), weight(whgt) { } ///< Constructor
        Edge(const Edge& other) ///< Copy constructor
        {
            idDest = other.idDest;
            weight = other.weight;
        }

        Edge operator=(const Edge& other) { return Edge(other); } ///< Assignment operator
    };

    /**
    * Encapsulates information about a graph vertex
    */
    struct Vertex
    {
        V info;
        mutable uint indegree;
        std::vector<Edge> adj;

        Vertex(const V& in) : info(in), indegree(0) { } ///< Constructor

        Vertex(const Vertex& other) ///< Copy constructor
        {
            info = other.info;
            indegree = other.indegree;
        }
    };

public:
    Graph(); ///< Constructor
    Graph(const Graph<V,E>& other); ///< Copy contructor
    ~Graph(); ///< Destructor

    uint AddVertex(const V& info); ///< Adds a vertex to the graph
    bool RemoveVertex(uint id); ///< Removes the specified vertex from the graph

    uint GetNumberOfVertices() const; ///< Returns the number of vertices
    std::vector<uint> GetVerticesIds() const; ///< Returns a vector with all the vertices ids

    bool AddEdge(uint sourceId, uint destId, const E& weight); ///< Adds an edge to the graph with the specified source and the specified destination
    bool RemoveEdge(uint sourceId, uint destId); ///< Removes the specified  edge

    /**
    * GetElement
    * @param id
    * @param elem variable where the value will be returned
    * @return true if the id is in use false otherwise
    */
    bool GetElement(uint id, V& elem);

    /**
     * GetElementInDegree Returns the number of edges that have the specified id as destination
     * @param id
     * @param elem variable where the value will be returned
     * @return true if the id is in use false otherwise
     */
    bool GetElementInDegree(uint id, int& elem);

    bool IsDag() const; ///< Checks if the graph is acyclic 
    virtual int GetNumCycles() const; ///< Returns the number of cycles of the graph

    /**
    * Performs a depth-first-search on the graph and returns a std::vector<uint> containing the ids by the order that were visited
    * @return ordered vector of ids
    */
    std::vector<uint> Dfs() const;

    virtual std::vector<uint> TopologicalOrder() const; ///< Returns a vector with the vertices ids ordered topologically
    virtual std::vector<uint> TopologicalOrder(uint srcId) const; ///< Returns a vector with the vertices ids ordered topologically forcing that the specified source comes in first
    void ResetIndegrees() const; ///< Resets the graph indegree values
    std::vector<uint> GetSources() const; ///< Returns all vertices of the graph that have indegree 0
    virtual DijkstraPath DijkstraShortestPath(uint srcId) const; ///< Returns the shortest path from the specified source to all the other vertices in the graph

protected:
    uint _nextId;
    std::map<uint, Vertex*> _vertices;
};

template <class V, class E>
std::vector<uint> Graph<V, E>::TopologicalOrder(uint srcId) const
{
    std::vector<uint> res;

    if(!IsDag())
        return res;

    ResetIndegrees();

    std::queue<uint> q;

    std::vector<uint> sources = GetSources();

    if (_vertices.at(srcId)->indegree != 0)
    {
        _vertices.at(srcId)->indegree = 0;
        sources.push_back(srcId);
    }
    else
    {
        auto sourceit = std::find(sources.begin(), sources.end(), srcId);
        q.push(*sourceit);
        sources.erase(sourceit);
    }

    while( !sources.empty() ) {
        q.push( sources.back() );
        sources.pop_back();
    }

    while(!q.empty()) {
        uint vertexId = q.front();
        q.pop();

        res.push_back(vertexId);

        for(const Edge& edge: _vertices.at(vertexId)->adj) {
            Vertex* dest = _vertices.at(edge.idDest);
            dest->indegree--;
            if( dest->indegree == 0)
                q.push( edge.idDest );
        }
    }

    /*if ( res.size() != _vertices.size() )
    res.erase(res.begin(), res.end());*/

    ResetIndegrees();

    return res;
}

template <class V, class E>
Graph<V, E>::~Graph()
{
    for (const auto& mv : _vertices)
        delete mv.second;
}

template <class V, class E>
bool Graph<V, E>::GetElementInDegree(uint id, int& elem)
{
    if (id >= _nextId) return false;
    if (_vertices.find(id) == _vertices.end()) return false;
    elem = _vertices[id]->indegree;
    return true;
}

template <class V, class E>
std::vector<uint> Graph<V, E>::GetVerticesIds() const
{
    std::vector<uint> res;

    for (const auto& ver: _vertices)
        res.push_back(ver.first);

    return res;
}

template <class V, class E>
std::vector<uint> Graph<V, E>::TopologicalOrder() const
{
    std::vector<uint> res;

    if(!IsDag())
        return res;

    ResetIndegrees();

    std::queue<uint> q;

    std::vector<uint> sources = GetSources();
    while( !sources.empty() ) {
        q.push( sources.back() );
        sources.pop_back();
    }

    while(!q.empty()) {
        uint vertexId = q.front();
        q.pop();

        res.push_back(vertexId);

        for(const Edge& edge: _vertices.at(vertexId)->adj) {
            Vertex* dest = _vertices.at(edge.idDest);
            dest->indegree--;
            if( dest->indegree == 0)
                q.push( edge.idDest );
        }
    }

    if ( res.size() != _vertices.size() )
        res.erase(res.begin(), res.end());

    ResetIndegrees();

    return res;
}

template <class V, class E>
Graph<V, E>::Graph() : _nextId(0) { }

template <class V, class E>
Graph<V, E>::Graph(const Graph<V,E>& other)
{
    for(const auto& ver : other._vertices)
        this->_vertices[ver.first] = new Vertex(*(ver.second));

    for (const auto& ver : other._vertices)
        for (const Edge& edge: ver.second->adj)
        {
            this->AddEdge(ver.first, edge.idDest, edge.weight);
        }
}

template <class V, class E>
uint Graph<V, E>::AddVertex(const V& info)
{
    _vertices[_nextId++] = new Vertex(info);
    return _nextId - 1;
}

template <class V, class E>
bool Graph<V, E>::RemoveVertex(uint id)
{
    if (id >= _nextId) return false;

    auto elem = _vertices.find(id);
    if (elem == _vertices.end()) return false;

    for (auto& edge : elem->second->adj)
        _vertices[edge.idDest]->indegree--;

    for (const std::pair<const uint, Vertex*>& ver: _vertices)
    {
        // Remove all edges that go into the vertex with the specified id
        ver.second->adj.erase(
            std::remove_if(
            ver.second->adj.begin(),
            ver.second->adj.end(),
            [id] (const Edge& e)
        {
            return e.idDest == id;
        }
        ), ver.second->adj.end());
    }

    delete elem->second;

    _vertices.erase(elem);

    return true;
}

template <class V, class E>
uint Graph<V, E>::GetNumberOfVertices() const
{
    return _vertices.size();
}

template <class V, class E>
bool Graph<V, E>::AddEdge(uint sourceId, uint destId, const E& weight)
{
    if (sourceId >= _nextId || destId >= _nextId) return false;

    auto sourceElem = _vertices.find(sourceId);
    if (sourceElem == _vertices.end()) return false;

    auto destElem = _vertices.find(destId);
    if (destElem == _vertices.end()) return false;

    auto pred = ([destId] (const Edge& e)
    {
        return e.idDest == destId;
    });

    if (std::find_if(sourceElem->second->adj.begin(), sourceElem->second->adj.end(), pred) != sourceElem->second->adj.end()) return false;

    sourceElem->second->adj.push_back(Edge(destId, weight));

    destElem->second->indegree++;

    return true;
}

template <class V, class E>
bool Graph<V, E>::RemoveEdge( uint sourceId, uint destId )
{
    if (sourceId >= _nextId || destId >= _nextId) return false;

    auto sourceElem = _vertices.find(sourceId);
    if (sourceElem == _vertices.end()) return false;

    auto destElem = _vertices.find(destId);
    if (destElem == _vertices.end()) return false;

    auto pred = ([destId] (const Edge& e)
    {
        return e.idDest == destId;
    });

    auto edg = std::find_if(sourceElem->second->adj.begin(), sourceElem->second->adj.end(), pred);
    if (edg == sourceElem->second->adj.end())
        return false;

    sourceElem->second->adj.erase(edg);
    destElem->second->indegree--;

    return true;
}

template <class V, class E>
bool Graph<V, E>::GetElement( uint id, V& elem )
{
    if (id >= _nextId) return false;
    if (_vertices.find(id) == _vertices.end()) return false;
    elem = _vertices[id]->info;
    return true;
}

template <class V, class E>
std::vector<uint> Graph<V, E>::Dfs() const
{
    std::map<uint, bool> visitedVertices;

    for (const auto& ver: _vertices)
        visitedVertices[ver.first] = false;

    std::vector<uint> result;

    std::stack<uint> stateStk;

    for (const std::pair<const uint, Vertex*>& elem: _vertices)
    {
        if (visitedVertices[elem.first]) continue;

        Vertex* vertex = elem.second;
        uint vertexId = elem.first;
        do
        {
            if (! visitedVertices[vertexId])
                result.push_back(vertexId);

            visitedVertices[vertexId] = true;

            for (const Edge& edge: reverse(vertex->adj))
            {
                if (! visitedVertices[edge.idDest])
                {
                    stateStk.push(edge.idDest);
                }
            }

            if (!stateStk.empty())
            {
                vertexId = stateStk.top();
                vertex = _vertices.at(vertexId);
                stateStk.pop();
            }
        } while (!stateStk.empty());
    }

    return result;
}

template <class V, class E>
bool Graph<V, E>::IsDag() const
{
    return this->GetNumCycles() == 0;
}

template <class V, class E>
int Graph<V, E>::GetNumCycles() const
{
    int result = 0;
    std::map<uint, bool> visitedVertices;
    std::map<uint, bool> processingVertices;

    for (const auto& ver: _vertices)
    {
        visitedVertices[ver.first] = false;
        processingVertices[ver.first] = false;
    }

    std::stack<uint> stateStk;

    for (const std::pair<const uint, Vertex*>& elem: _vertices)
    {
        Vertex* vertex = elem.second;
        uint vertexId = elem.first;
        do
        {
            visitedVertices[vertexId] = true;
            processingVertices[vertexId] = true;
            stateStk.push(vertexId);

            for (const Edge& edge: reverse(vertex->adj))
            {
                if (! visitedVertices[edge.idDest])
                {
                    stateStk.push(edge.idDest);
                }
                else if (processingVertices[edge.idDest])
                    result++;

            }

            while (!stateStk.empty())
            {
                vertexId = stateStk.top();
                vertex = _vertices.at(vertexId);
                stateStk.pop();

                if (processingVertices[vertexId])
                    processingVertices[vertexId] = false;
                else break;
            }
        } while (!stateStk.empty());
    }

    return result;
}

template <class V, class E>
void Graph<V,E>::ResetIndegrees() const
{
    for(const auto& ver: _vertices)
    {
        ver.second->indegree = 0;
    }

    for(const auto& ver: _vertices)
    {
        for (const Edge& edge: ver.second->adj)
        {
            _vertices.at(edge.idDest)->indegree++;
        }
    }
}

template <class V, class E>
std::vector<uint> Graph<V,E>::GetSources() const
{
    std::vector<uint> result;

    for (const auto& ver: _vertices)
        if (ver.second->indegree == 0)
            result.push_back(ver.first);

    return result;
}

template <class V, class E>
typename Graph<V,E>::DijkstraPath Graph<V,E>::DijkstraShortestPath(uint srcId) const
{
    struct VertexAux
    {
        VertexAux(uint p = std::numeric_limits<uint>::max(), double d = std::numeric_limits<double>::infinity(), bool pr = false) : pathId(p), dist(d), processing(pr) { }
        uint pathId;
        double dist;
        bool processing;
    };

    std::map<uint, VertexAux> vertexAux;

    for (const auto& ver : _vertices)
        vertexAux[ver.first] = VertexAux();

    uint v = srcId;
    vertexAux[v].dist = 0;

    std::vector<uint> pq;
    pq.push_back(v);

    std::make_heap(pq.begin(), pq.end());

    while (! pq.empty())
    {
        v = pq.front();
        pop_heap(pq.begin(), pq.end());
        pq.pop_back();

        VertexAux& vAux = vertexAux[v];
        vAux.processing = false;

        for (const Edge& edge : _vertices.at(v)->adj)
        {
            uint w = edge.idDest;
            VertexAux& wAux = vertexAux[w];

            if (vAux.dist + edge.weight.GetWeight() < wAux.dist)
            {
                wAux.dist = vAux.dist + edge.weight.GetWeight();
                wAux.pathId = v;

                if (! wAux.processing)
                {
                    wAux.processing = true;
                    pq.push_back(w);
                }

                std::make_heap (pq.begin(),pq.end());
            }
        }
    }

    DijkstraPath result;
    for (const auto& verAux : vertexAux)
    {
        /*DijkstraShortestPath::DijkstraVertex val(verAux.second.pathId, verAux.second.dist);*/
        result.Vertices.insert(std::make_pair(verAux.first, DijkstraPath::DijkstraVertex(verAux.second.pathId, verAux.second.dist)));
    }

    return result;
}

template<class V, class E>
typename Graph<V,E>::DijkstraPath::iterator Graph<V,E>::DijkstraPath::begin()
{
    return Vertices.begin();
}

template<class V, class E>
typename Graph<V,E>::DijkstraPath::iterator Graph<V,E>::DijkstraPath::end()
{
    return Vertices.end();
}

template<class V, class E>
typename Graph<V,E>::DijkstraPath::const_iterator Graph<V,E>::DijkstraPath::begin() const
{
    return Vertices.begin();
}

template<class V, class E>
typename Graph<V,E>::DijkstraPath::const_iterator Graph<V,E>::DijkstraPath::end() const
{
    return Vertices.end();
}

template <class V, class E>
std::vector<uint> Graph<V,E>::DijkstraPath::GetPath(uint source, uint destination) const
{
    std::vector<uint> result;

    uint current = destination;

    while(current != -1 && current != source)
    {
        result.push_back(current);
        current = Vertices.at(current).PathId;
    }

    if (current == -1)
        return std::vector<uint>();
    else
        result.push_back(current);

    std::reverse(result.begin(), result.end());
    return result;
}


#endif // GRAPH_H_
