#ifndef HYDOGRAPHIC_NETWORK_H_
#define HYDOGRAPHIC_NETWORK_H_

#include "graph.h"
#include "village.h"
#include "river.h"
#include "order.h"

#include <istream>
#include <vector>
#include <algorithm>
#include <limits>
#include <unordered_set>
#include <unordered_map>

struct RiverEdge
{
	RiverEdge(uint ri = std::numeric_limits<uint>::max(), bool ff = false) : RiverId(ri), FollowsFlow(ff) { }
	/*RiverEdge(const RiverEdge& other) : RiverId(other.RiverId), FollowsFlow(other.FollowsFlow) { }*/
	/*RiverEdge operator= (const RiverEdge& other) { return RiverEdge(other.RiverId, other.FollowsFlow); }*/
	uint RiverId;
	bool FollowsFlow;

	double GetWeight() const;
};

class HydographicNetwork : protected Graph<Village, RiverEdge>
{
public:
	struct Delivery
	{
		struct PathInfo
		{
			uint villageId;
			double transportedWeight;
			bool followsFlow;
			bool isIgarape;
		};
		Delivery(const HydographicNetwork* hn, const std::vector<PathInfo>& path) : hn(hn), Path(path) { }
		Delivery(const HydographicNetwork* hn, const std::vector<PathInfo>&& path) : hn(hn), Path(path) { }
		const std::vector<PathInfo> Path;
		const HydographicNetwork* hn;
		void ViewGraph() const;
	};
	
	HydographicNetwork() : _nextRiverId(0), _igarapeMaxCapacity(0.) { }

	Delivery GetDeliveryPath(uint src, std::unordered_multimap<uint, Order> orders, double boatCapacity, double supportVesselCapacity = 0.0, int numberOfSupportVessels = 0, double changeInRiverCapacity = 1.0);

    void ViewGraph() const;

	int getNumCycles() const override;
	std::vector<uint> topologicalOrder() const override;

	/*const Village& GetVillage(uint id) const;
	Village GetVillage(uint id);*/

	const River& GetRiver(uint id) const;

	uint AddVillage(const Village& info);
	bool RemoveVillage(uint id);

	uint AddRiver(uint sourceVilalge, uint destVillage, const River& weight);
	bool RemoveRiver(uint sourceId, uint destId);

	DijkstraShortestPath dijkstraShortestPath(uint srcId) const override;
	std::unordered_set<uint> GetVisitable(uint srcId) const;

private:
	typedef std::unordered_map<uint, River> RiversContainer;
	RiversContainer _rivers;
	uint _nextRiverId;
	double _igarapeMaxCapacity;
};

#endif // HYDOGRAPHIC_NETWORK_H_
