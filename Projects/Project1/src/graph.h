#ifndef GRAPH_H_
#define GRAPH_H_

#include "defs.h"
#include "utilities.h"

#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <algorithm>
#include <exception>

template <class V, class E>
class Graph
{
// Inner Classes Declarations
private:
	struct Edge
	{
		uint idDest;
		E weight;

		Edge(uint destId, const E& whgt)
		{
			idDest = destId;
			weight = whgt;
		}
	};

	struct Vertex
	{
		V info;
		mutable uint indegree;
		std::vector<Edge> adj;

		Vertex(const V& in)
		{
			info = in;
			indegree = 0;
		}
	};

public:
	Graph();

	uint AddVertex(const V& info);
	bool RemoveVertex(uint id);

	uint GetNumberOfVertices() const;
	std::vector<uint> GetVerticesIds() const;

	bool AddEdge(uint sourceId, uint destId, const E& weight);
	bool RemoveEdge(uint sourceId, uint destId);

	/**
	 * GetElement
	 * @param id
	 * @param elem variable where the value will be returned
	 * @return true if the id is in use false otherwise
	 */
	bool GetElement(uint id, V& elem);
	bool GetElementInDegree(uint id, int& elem);
	bool isDag() const;
	int getNumCycles() const;

	/**
	 * Performs a depth-first-search on the graph and returns a std::vector<uint> containing the ids by the order that were visited
	 * @return ordered vector of ids
	 */
	std::vector<uint> dfs() const;
	std::vector<uint> topologicalOrder() const;
	void resetIndegrees() const;
	std::vector<uint> getSources() const;
	std::map<uint, uint> dijkstraShortestPath(uint srcId) const;
private:
	uint _nextId;
	std::map<uint, Vertex*> _vertices;
};

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
std::vector<uint> Graph<V, E>::topologicalOrder() const
{
	std::vector<uint> res;

	if( ! isDag() )
		return res;

	resetIndegrees();

	std::queue<uint> q;

	std::vector<uint> sources = getSources();
	while( !sources.empty() ) {
		q.push( sources.back() );
		sources.pop_back();
	}

	while( !q.empty() ) {
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
	
	resetIndegrees();

	return res;
}

template <class V, class E> 
Graph<V, E>::Graph() : _nextId(0) { }

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
std::vector<uint> Graph<V, E>::dfs() const
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

#endif // GRAPH_H_

template <class V, class E>
bool Graph<V, E>::isDag() const
{
	return getNumCycles() == 0;
}

template <class V, class E>
int Graph<V, E>::getNumCycles() const
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
void Graph<V,E>::resetIndegrees() const 
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
std::vector<uint> Graph<V,E>::getSources() const
{
	std::vector<uint> result;

	for (const auto& ver: _vertices)
		if (ver.second->indegree == 0)
			result.push_back(ver.first);

	return result;
}

template <class V, class E>
std::map<uint, uint> Graph<V,E>::dijkstraShortestPath(uint srcId) const
{
	struct VertexAux
	{
		VertexAux(uint p = std::numeric_limits<uint>::max(), double d = std::numeric_limits<double>::infinity(), bool pr = false) : pathID(p), dist(d), processing(pr) { }
		uint pathID;
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

			if (vAux.dist + edge.weight < wAux.dist)
			{
				 wAux.dist = vAux.dist + edge.weight;
				 wAux.pathID = v;

				 if (! wAux.processing)
				 {
					 wAux.processing = true;
					 pq.push_back(w);
				 }

				std::make_heap (pq.begin(),pq.end());//,[&vertexAux](uint arg0, uint arg1) { return vertexAux[arg0].dist > vertexAux[arg1].dist; });
			}
		}
	}

	std::map<uint, uint> result;

	for (const auto& verAux : vertexAux)
		result[verAux.first] = verAux.second.pathID;

	return result;

}
