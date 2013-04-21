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
		Delivery(const HydographicNetwork* hn, const std::vector<uint>& path, double consumption) : hn(hn), Path(path), TotalConsumption(consumption) { }
		const std::vector<uint> Path;
		const double TotalConsumption;
		const HydographicNetwork* hn;
		void ViewGraph() const;
	};

	HydographicNetwork() : _nextRiverId(0) { }

	Delivery GetDeliveryPath(uint src, std::multimap<uint, Order> orders);

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

private:
	std::map<uint, River> _rivers;
	uint _nextRiverId;
};

#endif // HYDOGRAPHIC_NETWORK_H_
