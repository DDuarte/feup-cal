#include "hydographic_network.h"
#include "utilities.h"

#include "graphviewer.h"

#include <numeric>

HydographicNetwork::Delivery HydographicNetwork::GetDeliveryPath(uint src, std::unordered_multimap<uint, Order> orders, double boatCapacity, double supportVesselCapacity, int numberOfSupportVessels, double changeInRiverCapacity)
{
	std::vector<Delivery::PathInfo> path;

	_igarapeMaxCapacity = (numberOfSupportVessels == 0 ? boatCapacity : supportVesselCapacity);

	std::unordered_set<uint> visitable = GetVisitable(src); // O(|V|+|E|)

	for (std::unordered_multimap<uint, Order>::iterator iter = orders.begin(); iter != orders.end();) //
	{																						//
		auto erase_iter = iter++;															//
																							// O(|N|lg(|N|))
		// removes all even values															//
		if (visitable.find(erase_iter->first) == visitable.end())							//
			orders.erase(erase_iter);														//
	}

	double totalWeight = std::accumulate(orders.begin(), orders.end(), 0.0,   // 
		[] (double val, std::pair<const uint, Order> o1)					  // 
	{																		  // O(|N|)
		return val + o1.second.GetWeight();									  // 
	});																		  // 

	size_t srcOrdersNumber = orders.count(src);
	if (srcOrdersNumber > 0) 
	{
		for (size_t i = 0; i < srcOrdersNumber; ++i)
		{
			std::unordered_multimap<uint, Order>::iterator srcOrder = orders.find(src);
			totalWeight -= srcOrder->second.GetWeight();
			orders.erase(srcOrder);
		}
	}

	if (changeInRiverCapacity != 1.0)
	{
		for (RiversContainer::iterator rit = _rivers.begin(); rit != _rivers.end(); rit++)
		{
			rit->second.SetMaxCapacity(rit->second.GetMaxCapacity() * changeInRiverCapacity);
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

		size_t path1Size = path1.size();
		path1Size = (path1Size == 0 ? 0 : path1Size - 1);
		for (size_t j = 0; j < path1Size; ++j)
		{
			Delivery::PathInfo pi;
			pi.villageId = path1[j];
			pi.transportedWeight = totalWeight;
			
			const Vertex& ver = *(_vertices.at(path1[j]));

			uint nextVerId = path1[j+1];

			std::vector<Edge>::const_iterator edge = std::find_if(ver.adj.cbegin(), ver.adj.cend(), [nextVerId] (const Edge& e) { return e.idDest == nextVerId; });
			pi.followsFlow = edge->weight.FollowsFlow;
			
			const River& river = _rivers.at(edge->weight.RiverId);
			pi.isIgarape = (river.GetMaxCapacity() >= supportVesselCapacity && river.GetMaxCapacity() < boatCapacity);

			path.push_back(pi);
		}

		dijkstraSrc = *nextElem;

		if (dijkstraSrc != src)
		{
			size_t numOrders = orders.count(dijkstraSrc);
			for (size_t i = 0; i < numOrders; ++i)
			{
				std::unordered_multimap<uint, Order>::iterator srcOrder = orders.find(dijkstraSrc);
				totalWeight -= srcOrder->second.GetWeight();
				orders.erase(srcOrder);
			}
		}

		if (path1.size() != 0 && i == (number_of_times - 1))
		{
			Delivery::PathInfo pi;
			pi.villageId = path1.back();
			pi.transportedWeight = 0.;
			path.push_back(pi);
		}


	}

	if (changeInRiverCapacity != 1.0)
	{
		double invChangeInRiverCapacity = 1.0 / changeInRiverCapacity;
		for (RiversContainer::iterator rit = _rivers.begin(); rit != _rivers.end(); rit++)
		{
			rit->second.SetMaxCapacity(rit->second.GetMaxCapacity() * invChangeInRiverCapacity);
		}
	}

	_igarapeMaxCapacity = 0.;

	return Delivery(this, std::move(path));
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
			if (edge.weight.FollowsFlow && _rivers.at(edge.weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
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
			if (edge.weight.FollowsFlow && _rivers.at(edge.weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
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
				if (edgeIt->weight.FollowsFlow && _rivers.at(edgeIt->weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
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
	GraphViewer gv(600, 600, true);
	gv.createWindow(600, 600);

	gv.defineVertexColor(DARK_GRAY);
	gv.defineEdgeColor(BLUE);

	int edgeId = 1;
	std::map<const Edge*, int> edges;

	for (std::map<uint, Vertex*>::value_type v : hn->_vertices)
	{
		gv.addNode(v.first);
		gv.setVertexLabel(v.first, v.second->info.GetName());

		for (Edge& e : v.second->adj)
		{
			gv.addEdge(edgeId, v.first, e.idDest, EdgeType::DIRECTED);
			gv.setEdgeThickness(edgeId, 50 / hn->GetRiver(e.weight.RiverId).GetMaxCapacity());
			gv.setEdgeLabel(edgeId, hn->GetRiver(e.weight.RiverId).GetName());

			edges.insert(std::make_pair(&e, edgeId));

			edgeId += 1;
		}
	}

	std::vector<PathInfo>::const_iterator finalVertex = Path.begin() + Path.size() - 1;
	for (std::vector<PathInfo>::const_iterator verIdIt = Path.begin(); verIdIt != finalVertex; verIdIt++)
	{
		const Vertex& ver = *(hn->_vertices.at(verIdIt->villageId));
		std::vector<PathInfo>::const_iterator nextVertexId = verIdIt + 1;
		for (const Edge& e : ver.adj)
		{
			if (e.idDest == nextVertexId->villageId)
			{
				gv.setEdgeColor(edges.at(&e), BLACK);
				break;
			}
		}
	}


	

	gv.rearrange();
}

Graph<Village, RiverEdge>::DijkstraShortestPath HydographicNetwork::dijkstraShortestPath(uint srcId) const 
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

		for (std::vector<Edge>::const_reference edge : _vertices.at(v)->adj)
		{
			if (_rivers.at(edge.weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
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
	}

	DijkstraShortestPath result;
	for (const auto& verAux : vertexAux) 
	{
		/*DijkstraShortestPath::DijkstraVertex val(verAux.second.pathId, verAux.second.dist);*/
		result.Vertices.insert(std::make_pair(verAux.first, DijkstraShortestPath::DijkstraVertex(verAux.second.pathId, verAux.second.dist)));
	}

	return result;
}

std::unordered_set<uint> HydographicNetwork::GetVisitable(uint srcId) const
{
	std::unordered_set<uint> result;
	std::queue<uint> vers;
	vers.push(srcId);

	while(!vers.empty())
	{
		uint ver = vers.front();
		vers.pop();

		result.insert(ver);

		for (std::vector<Edge>::const_reference edge : _vertices.at(ver)->adj)
		{
			if ((result.find(edge.idDest) == result.end()) && _rivers.at(edge.weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
			{
				vers.push(edge.idDest);
			}
		}
	}

	return result;
}
