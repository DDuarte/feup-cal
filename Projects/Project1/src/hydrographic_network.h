#ifndef HYDOGRAPHIC_NETWORK_H_
#define HYDOGRAPHIC_NETWORK_H_

#include "graph.h"
#include "village.h"
#include "river.h"
#include "order.h"
#include "delivery.h"

#include <istream>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>
#include <unordered_set>
#include <unordered_map>
#include <set>

class GraphViewer;

struct RiverEdge
{
    RiverEdge() : Distance(-1), RiverId(std::numeric_limits<uint>::max()), FollowsFlow(false) { }
    RiverEdge(double d, uint ri, bool ff) : Distance(d), RiverId(ri), FollowsFlow(ff) { }
    uint RiverId;
    bool FollowsFlow;
    double Distance;

    double GetWeight() const
    {
        if (FollowsFlow)
            return Distance;
        else
            return 2.0 * Distance;
    }
};

class HydrographicNetwork : protected Graph<Village, RiverEdge>
{
public:
    HydrographicNetwork() : _nextRiverId(0), _tempEdgeId(1), _igarapeMaxCapacity(0.), _graphViewer(nullptr) { }
    ~HydrographicNetwork();

    static bool Load(std::istream& source, HydrographicNetwork& hn);

    Delivery GetDeliveryItinerary(uint src, std::unordered_map<uint, std::vector<Order>> orders, double boatCapacity, double supportVesselCapacity = 0.0, uint numberOfSupportVessels = 0);
    Delivery GetDeliveryPath(uint src, std::unordered_map<uint, std::vector<Order>> orders, double boatCapacity, double supportVesselCapacity = 0.0, uint numberOfSupportVessels = 0);

    void ViewGraph();
    void ViewGraph(Delivery& delivery, const std::string& color);

    int getNumCycles() const override;
    std::vector<uint> topologicalOrder() const override;

    const River& GetRiver(uint id) const { return _rivers.at(id); }

    uint AddVillage(const Village& info) { return AddVertex(info); }
    bool RemoveVillage(uint id) { return RemoveVertex(id); }

    uint AddRiver(uint sourceVilalge, uint destVillage, const River& weight);
    bool RemoveRiver(uint sourceId, uint destId) { return RemoveEdge(sourceId, destId); }

    DijkstraShortestPath dijkstraShortestPath(uint srcId) const override;
    std::unordered_set<uint> GetVisitable(uint srcId) const;

    GraphViewer* GetGraphViewer() const { return _graphViewer; }

    void ChangeRiversCapacity(double factor);

private:
    typedef std::unordered_map<uint, River> RiversContainer;
    RiversContainer _rivers;
    uint _nextRiverId;
    uint _tempEdgeId;
    double _igarapeMaxCapacity;
    GraphViewer* _graphViewer;
};

#endif // HYDOGRAPHIC_NETWORK_H_
