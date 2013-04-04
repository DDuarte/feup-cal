#ifndef GRAPH_H_
#define GRAPH_H_

#include "defs.h"

#include <vector>
#include <map>
#include <algorithm>

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
		uint indegree;
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

	bool AddEdge(uint sourceId, uint destId, const E& weight);
	bool RemoveEdge(uint sourceId, uint destId);

	/**
	 * GetElement
	 * @param id
	 * @param elem variable where the value will be returned
	 * @return true if the id is in use false otherwise
	 */
	bool GetElement(uint id, V& elem);

	/**
	 * Performs a depth-first-search on the graph and returns a std::vector<uint> containing the ids by the order that were visited
	 * @return ordered vector of ids
	 */
	std::vector<uint> dfs() const
private:
	uint _nextId;
	std::map<uint, Vertex*> _vertices;
};

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
	elem = _vertices[id];
	return true;
}

template <class V, class E>
std::vector<uint> Graph<V, E>::dfs() const
{
	throw std::exception("The method or operation is not implemented.");
}

#endif // GRAPH_H_
