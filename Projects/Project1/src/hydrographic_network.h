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
class Menu;
class ByteBuffer;

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
    typedef std::unordered_map<uint, std::pair<uint, uint>> BoatMap;
    typedef std::vector<uint> Path;
    typedef std::unordered_map<uint, Path> DeliveryMap;
    typedef std::unordered_set<uint> VertexSet;
    typedef std::unordered_map<uint, River> RiversContainer;

    HydrographicNetwork(const std::string& name) : _name(name), _nextRiverId(0), _tempEdgeId(1), _igarapeMaxCapacity(0.), _graphViewer(nullptr) { }
    ~HydrographicNetwork();

    DeliveryRoute GetDeliveryItinerary(Delivery& delivery);
    DeliveryRoute GetDeliveryPath(Delivery& delivery);

    void ViewGraph();
    void ViewGraph(DeliveryRoute& DeliveryRoute, const std::string& color);

    int getNumCycles() const override;
    std::vector<uint> topologicalOrder() const override;

    uint AddVillage(const Village& info) { return AddVertex(info); }
    bool RemoveVillage(uint id) { return RemoveVertex(id); }
    const Village* GetVillage(uint id) const;

    uint AddRiver(uint sourceVillage, uint destVillage, const River& river);
    bool RemoveRiver(uint sourceId, uint destId) { return RemoveEdge(sourceId, destId); }
    const River* GetRiver(uint id) const;

    DijkstraShortestPath dijkstraShortestPath(uint srcId) const override;
    std::unordered_set<uint> GetVisitable(uint srcId) const;

    GraphViewer* GetGraphViewer() const { return _graphViewer; }

    static Menu* GetMenu() { return _menu; } ///< Returns the menu for the HydrographicNetwork class

    void ChangeRiversCapacity(double factor);

    static HydrographicNetwork* Load(ByteBuffer& bb);

private:
    RiversContainer _rivers;
    uint _nextRiverId;
    uint _tempEdgeId;
    double _igarapeMaxCapacity;
    GraphViewer* _graphViewer;
    std::string _name;

    Delivery::OrderMap FilterUnreachable(uint src, Delivery::OrderMap& orders);

    static Menu* _menu; ///< Menu associated with the HydrographicNetwork class
};

#endif // HYDOGRAPHIC_NETWORK_H_
