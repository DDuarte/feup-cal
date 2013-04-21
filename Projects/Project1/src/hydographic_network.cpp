#include "hydographic_network.h"
#include "utilities.h"

#include "graphviewer.h"

#include <numeric>

HydographicNetwork::Delivery HydographicNetwork::GetDeliveryPath(uint src, std::multimap<uint, Order> orders)
{
	std::vector<uint> path;

	double consumption = 0.0;

	double totalWeight = std::accumulate(orders.begin(), orders.end(), 0.0, 
		[] (double val, std::multimap<uint, Order>::value_type o1)
	{
		return val + o1.second.GetWeight();
	});

	path.push_back(src);

	size_t srcOrdersNumber = orders.count(src);
	if (srcOrdersNumber > 0) 
	{
		for (size_t i = 0; i < srcOrdersNumber; ++i)
		{
			std::multimap<uint, Order>::iterator srcOrder = orders.find(src);
			totalWeight -= srcOrder->second.GetWeight();
			orders.erase(srcOrder);
		}
	}

	std::vector<uint> topoOrder = topologicalOrder();


	size_t number_of_times = orders.size();
	uint dijkstraSrc = src;
	for (size_t i = 0; i < number_of_times; ++i) 
	{
		DijkstraShortestPath shrtPath = dijkstraShortestPath(dijkstraSrc);

		double minDist = std::numeric_limits<double>::infinity();

		std::vector<uint> minimumDists;

		for (DijkstraShortestPath::const_reference elem : shrtPath)
			if (elem.first != dijkstraSrc && (orders.find(elem.first) != orders.end())) 
				if (elem.second.Dist < minDist)
				{
					minDist = elem.second.Dist;
					minimumDists.erase(minimumDists.begin(), minimumDists.end());
					minimumDists.push_back(elem.first);
				}
				else if (elem.second.Dist == minDist)
					minimumDists.push_back(elem.first);

		auto nextElem = std::min_element(minimumDists.begin(), minimumDists.end(), [ &topoOrder](uint const& r, uint const& s){
			auto i = std::distance( topoOrder.begin(), std::find(topoOrder.begin(), topoOrder.end(), r)); 
			auto j = std::distance( topoOrder.begin(), std::find(topoOrder.begin(), topoOrder.end(), s)); 
			return i < j;
		});

		std::vector<uint> path1 = shrtPath.GetPath(dijkstraSrc, *nextElem);
		consumption += totalWeight * shrtPath.Vertices.at(*nextElem).Dist;

		path.insert(path.end(), path1.begin() + 1, path1.end());

		dijkstraSrc = *nextElem;

		if (dijkstraSrc != src)
		{
			size_t numOrders = orders.count(dijkstraSrc);
			for (size_t i = 0; i < numOrders; ++i)
			{
				std::multimap<uint, Order>::iterator srcOrder = orders.find(dijkstraSrc);
				totalWeight -= srcOrder->second.GetWeight();
				orders.erase(srcOrder);
			}
		}


	}

	return Delivery(this, path, consumption);
}

void HydographicNetwork::ViewGraph() const
{
	GraphViewer gv(600, 600, true);
	gv.createWindow(600, 600);

	gv.defineVertexColor(DARK_GRAY);
	gv.defineEdgeColor(BLUE);

	int edgeId = 1;

	for (std::map<uint, Vertex*>::value_type v : _vertices)
	{
		gv.addNode(v.first);
		gv.setVertexLabel(v.first, v.second->info.GetName());

		for (Edge& e : v.second->adj)
		{
			gv.addEdge(edgeId, v.first, e.idDest, EdgeType::DIRECTED);
			gv.setEdgeThickness(edgeId, 50 / GetRiver(e.weight.RiverId).GetMaxCapacity());
			gv.setEdgeLabel(edgeId, GetRiver(e.weight.RiverId).GetName());

			edgeId += 1;
		}
	}

	gv.rearrange();
}

std::vector<uint> HydographicNetwork::topologicalOrder() const 
{
	std::vector<uint> res;

	if(!isDag())
		return res;

	for (const auto& ver : _vertices)
		ver.second->indegree = 0;

	for (const auto& ver : _vertices)
		for (std::vector<Edge>::const_reference edge : ver.second->adj)
		{
			if (edge.weight.FollowsFlow)
				_vertices.at(edge.idDest)->indegree++;
		}

	std::queue<uint> q;

	std::vector<uint> sources = getSources();
	while( !sources.empty() ) {
		q.push( sources.back() );
		sources.pop_back();
	}

	while(!q.empty()) {
		uint vertexId = q.front();
		q.pop();

		res.push_back(vertexId);

		for(const Edge& edge: _vertices.at(vertexId)->adj) 
		{
			if (edge.weight.FollowsFlow)
			{
				Vertex* dest = _vertices.at(edge.idDest);
				dest->indegree--;
				if( dest->indegree == 0)
					q.push( edge.idDest );
			}
		}
	}

	if ( res.size() != _vertices.size() )
		res.erase(res.begin(), res.end());

	resetIndegrees();

	return res;
}

int HydographicNetwork::getNumCycles() const
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

	for (std::map<uint, Vertex*>::const_reference elem: _vertices)
	{
		Vertex* vertex = elem.second;
		uint vertexId = elem.first;
		do
		{
			visitedVertices[vertexId] = true;
			processingVertices[vertexId] = true;
			stateStk.push(vertexId);

			for (std::vector<Edge>::reverse_iterator edgeIt = vertex->adj.rbegin(); edgeIt != vertex->adj.rend(); edgeIt++)
			{
				if (edgeIt->weight.FollowsFlow)
				{
					if (! visitedVertices[edgeIt->idDest])
					{
						stateStk.push(edgeIt->idDest);
					}
					else if (processingVertices[edgeIt->idDest])
						result++;
				}

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

uint HydographicNetwork::AddVillage(const Village& info)
{
	return AddVertex(info);
}

bool HydographicNetwork::RemoveVillage(uint id)
{
	return RemoveVertex(id);
}

uint HydographicNetwork::AddRiver(uint sourceVilalge, uint destVillage, const River& weight)
{
	uint riverId = _nextRiverId++;
	_rivers.insert(std::make_pair(riverId, weight));

	if (AddEdge(sourceVilalge, destVillage, RiverEdge(riverId, true)))
		if (AddEdge(destVillage, sourceVilalge, RiverEdge(riverId, false)))
			return riverId;

	_rivers.erase(riverId);
	_nextRiverId--;

	return -1;
}

bool HydographicNetwork::RemoveRiver(uint sourceId, uint destId)
{
	return RemoveEdge(sourceId, destId);
}

const River& HydographicNetwork::GetRiver(uint id) const
{
	return _rivers.at(id);
}

double RiverEdge::GetWeight() const
{
	return 1.0 + !FollowsFlow;
}

void HydographicNetwork::Delivery::ViewGraph() const
{

}
