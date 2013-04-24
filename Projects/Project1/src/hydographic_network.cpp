#include "hydographic_network.h"
#include "utilities.h"

#include "graphviewer.h"

#include <numeric>
#include <set>
#include <cmath>

HydrographicNetwork::Delivery HydrographicNetwork::GetDeliveryPath(uint src, std::unordered_map<uint, std::vector<Order>> orders, double boatCapacity, double supportVesselCapacity, int numberOfSupportVessels, double changeInRiverCapacity)
{
	std::vector<Delivery::PathInfo> path;

	if (changeInRiverCapacity != 1.0)
	{
		for (RiversContainer::iterator rit = _rivers.begin(); rit != _rivers.end(); rit++)
		{
			rit->second.SetMaxCapacity(rit->second.GetMaxCapacity() * changeInRiverCapacity);
		}
	}

	_igarapeMaxCapacity = boatCapacity;

	std::unordered_set<uint> visitable = GetVisitable(src); // O(|V|+|E|)

	std::unordered_map<uint, std::vector<Order>> unreacheable;

	std::swap(unreacheable, orders);
	for (uint verId : visitable)
	{
		auto val = unreacheable.find(verId);
		if (val != unreacheable.end())
		{
			orders.insert(std::make_pair(verId, std::move(unreacheable[verId])));
			unreacheable.erase(verId);
		}
	}

	std::unordered_map<uint, std::vector<Order>> reachableWithIgarapes;

	if (numberOfSupportVessels > 0 && !unreacheable.empty())
	{
		_igarapeMaxCapacity = supportVesselCapacity;

		std::unordered_set<uint> visitableWithIgarapes = GetVisitable(src);

		for (uint verId : visitableWithIgarapes)
		{
			auto val = unreacheable.find(verId);
			if (val != unreacheable.end())
			{
				reachableWithIgarapes.insert(std::make_pair(verId, std::move(unreacheable[verId])));
				unreacheable.erase(verId);
			}
		}
	}

	std::unordered_map<uint, std::vector<Order>>::iterator it = orders.find(src);
	if (it != orders.end()) 
	{
		orders.erase(it);
	}

	double totalWeight = std::accumulate(orders.begin(), orders.end(), 0.0,  
		[] (double val, std::pair<const uint, std::vector<Order>> o1)					  
	{							
		for (const Order& ord : o1.second)
			val += ord.GetWeight();	
		
		return val; 
	});												  

	/*for (std::unordered_map<uint, std::vector<Order>>::const_reference ord : orders)
	{
		std::unordered_set<uint> ords_i;
		for (uint i = 0; i < ord.second.size(); ++i)
			ords_i.insert(i);


		KnapsackSolver(boatCapacity, ord.second, ords_i);
	}*/

	std::unordered_map<uint, uint> numberOfBoats; 
	for(std::unordered_map<uint, std::vector<Order>>::const_reference ord : orders)
	{
		std::vector<Order> ords = ord.second; 
		uint ordersVolume = std::accumulate(ords.begin(), ords.end(), 0, [] (double val, const Order & order)					  
		{							
			return val + order.GetVolume(); 
		});

		numberOfBoats.insert(std::make_pair(ord.first, ceill(ordersVolume / static_cast<double>(boatCapacity))));
	}

	std::unordered_map<uint, std::vector<Order>>::iterator ord;
	for(std::unordered_map<uint, std::vector<Order>>::iterator ord = reachableWithIgarapes.begin(); ord != reachableWithIgarapes.end() && numberOfSupportVessels > 0; ord++)
	{
		std::vector<Order> ords = ord->second; 
		double ordersVolume;
		double boatCap = static_cast<double>(boatCapacity);
		
		for(std::vector<Order>::iterator orderIt = ords.begin(); orderIt != ords.end(); orderIt++)
		{
			double vol = orderIt->GetVolume() / boatCap;

			if(ordersVolume + vol >= numberOfSupportVessels)
			{
				std::vector<Order> toRemove(orderIt, ords.end());
				unreacheable.insert(std::make_pair(ord->first, std::move(toRemove)));
				ords.erase(orderIt, ords.end());
				break;
			}
			else
				ordersVolume += vol; 

			
		}

		uint boats = ceill(ordersVolume);

		numberOfSupportVessels -= boats;
		
		numberOfBoats.insert(std::make_pair(ord->first, boats));
	}

	if (ord != reachableWithIgarapes.end())
	{
		unreacheable.insert(ord, reachableWithIgarapes.end());
		reachableWithIgarapes.erase(ord, reachableWithIgarapes.end());
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

		std::unordered_map<uint, std::vector<Order>>::iterator srcOrderIt;
		if (dijkstraSrc != src && ((srcOrderIt = orders.find(dijkstraSrc)) != orders.end()))
		{
			for (const Order& ord : srcOrderIt->second)
			{
				totalWeight -= ord.GetWeight();
			}
			orders.erase(srcOrderIt);
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

void HydrographicNetwork::ViewGraph() const
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

std::vector<uint> HydrographicNetwork::topologicalOrder() const 
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

int HydrographicNetwork::getNumCycles() const
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

uint HydrographicNetwork::AddVillage(const Village& info)
{
	return AddVertex(info);
}

bool HydrographicNetwork::RemoveVillage(uint id)
{
	return RemoveVertex(id);
}

uint HydrographicNetwork::AddRiver(uint sourceVillage, uint destVillage, const River& weight)
{
	uint riverId = _nextRiverId++;	

	if (AddEdge(sourceVillage, destVillage, RiverEdge(riverId, true)))
	{
		if (!AddEdge(destVillage, sourceVillage, RiverEdge(riverId, false)))
		{
			RemoveEdge(sourceVillage, destVillage);
			_nextRiverId--;
			return -1;
		}
	}

	_rivers.insert(std::make_pair(riverId, weight));
	return riverId;
}

bool HydrographicNetwork::RemoveRiver(uint sourceId, uint destId)
{
	return RemoveEdge(sourceId, destId);
}

const River& HydrographicNetwork::GetRiver(uint id) const
{
	return _rivers.at(id);
}

double RiverEdge::GetWeight() const
{
	return 1.0 + !FollowsFlow;
}

void HydrographicNetwork::Delivery::ViewGraph() const
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

Graph<Village, RiverEdge>::DijkstraShortestPath HydrographicNetwork::dijkstraShortestPath(uint srcId) const 
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

std::unordered_set<uint> HydrographicNetwork::GetVisitable(uint srcId) const
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

std::unordered_set<uint> HydrographicNetwork::KnapsackSolver(double max_capacity, const std::vector<Order>& orders, std::unordered_set<uint>& orders_i)
{
	std::vector<std::vector<uint>> values(orders_i.size() + 1, std::vector<uint>(max_capacity + 1, 0));
	std::vector<std::vector<uint>> best(orders_i.size() + 1, std::vector<uint>(max_capacity + 1, 0));

	uint i = 1;
	for (uint ord_i : orders_i)
	{
		for (uint j = 1; j < max_capacity; ++j)
		{
			if (orders[ord_i].GetVolume() <= j)
			{
				if (orders[ord_i].GetVolume() + values[i-1][j - orders[ord_i].GetVolume()] > values[i-1][j])
				{
					values[i][j] = orders[i].GetVolume() + values[i-1][j - orders[ord_i].GetVolume()];
					best[i][j] = i;
				}
				else
				{
					values[i][j] = values[i-1][j];
					best[i][j] = best[i-1][j];
				}

			}
			else
			{
				values[i][j] = values[i-1][j];
				best[i][j] = best[i-1][j];
			}
		}
		++i;
	}

	uint volume = max_capacity;
	uint item = orders_i.size() - 1;

	std::unordered_set<uint> result;

	while (item > 0 && best[item][volume] != 0)
	{
		result.insert(best[item][volume]);
		orders_i.erase(best[item][volume]);
		volume -= orders[ best[item][volume] ].GetVolume();
		item--;
	}

	return result;
}

