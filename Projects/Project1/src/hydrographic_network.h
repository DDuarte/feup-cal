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

//! RiverEdge class
/*!
    Represents an edge in the graph. It is used to distinguish the flow's way.
*/
struct RiverEdge
{
    RiverEdge() : Distance(-1), RiverId(std::numeric_limits<uint>::max()), FollowsFlow(false) { } ///< Constructor
    RiverEdge(double d, uint ri, bool ff) : Distance(d), RiverId(ri), FollowsFlow(ff) { } ///< Constructor

    uint RiverId;
    bool FollowsFlow;
    double Distance;

    double GetWeight() const ///< Returns the Edge Weight
    {
        if (FollowsFlow)
            return Distance;
        else
            return 2.0 * Distance;
    }
};


/**
* Represents an Hydrographic Network
*/
class HydrographicNetwork : protected Graph<Village, RiverEdge>
{
public:
    typedef std::vector<uint> Path;
    typedef std::unordered_map<uint, Path> DeliveryMap;
    typedef std::unordered_set<uint> VertexSet;
    typedef std::unordered_map<uint, River> RiversContainer;

    HydrographicNetwork(const std::string& name) : _name(name), _nextRiverId(0), _igarapeMaxCapacity(0.), _graphViewer(nullptr) { } ///< Constructor
    ~HydrographicNetwork(); ///< Destructor

    DeliveryRoute GetDeliveryItinerary(Delivery& delivery); ///< Calculates and returns one itinerary for the specified delivery.
    DeliveryRoute GetDeliveryPath(Delivery& delivery); ///< Calculates and returns one path per destination for the specified delivery

    void ViewGraph(); ///< Shows the HydrographicNetwork with the GraphViewer API
    void ViewGraph(DeliveryRoute& deliveryRoute); ///< Shows the HydrographicNetwork and the specified DeliveryRoute with the GraphViewer API

    int GetNumCycles() const override; ///< Returns the number of cycles of the HydrographicNetwork
    std::vector<uint> TopologicalOrder() const override; ///< Returns the vertices of the graph ordered topologically

    void AddRiverToGraphViewer(uint id, uint src, uint dest, const River& river) const;
    void AddVillageToGraphViewer(uint id, const Village& info /*, double dX, double dY */) const;

    uint AddVillage(const Village& info); ///< Adds a village to the HydrographicNetwork
    bool RemoveVillage(uint id); ///< Removes a village to the HydrographicNetwork
    const Village* GetVillage(uint id) const; ///< Returns the Village with the specified id

    uint AddRiver(uint sourceVillage, uint destVillage, const River& river); ///< Adds a river to the HydrographicNetwork
    bool RemoveRiver(uint sourceId, uint destId) { return RemoveEdge(sourceId, destId); } ///< Removes a river to the HydrographicNetwork
    const River* GetRiver(uint id) const; ///< Returns the river with the specified id

    DijkstraPath DijkstraShortestPath(uint srcId) const override; ///< Returns the dijkstra shortest path from the specified source village
    std::unordered_set<uint> GetVisitable(uint srcId) const; ///< Returns the visitable vertices ids from the specified source village

    GraphViewer* GetGraphViewer() const { return _graphViewer; } ///< Returns the GraphViewer class pointer

    static Menu* GetMenu() { return _menu; } ///< Returns the menu for the HydrographicNetwork class

    void ChangeRiversCapacity(double factor); ///< Changes the rivers capacity multiplying all the rivers capacities by the specified factor

    const std::string& GetName() const { return _name; }

    static HydrographicNetwork* Load(ByteBuffer& bb); ///< Loads an HydrographicNetwork from the specified ByteBuffer instance

    void Save(ByteBuffer& bb);

private:
    RiversContainer _rivers;
    uint _nextRiverId;
    double _igarapeMaxCapacity;
    GraphViewer* _graphViewer;
    std::string _name;

    /**
     * Filters the unreachable orders from the specified OrderMap. The function removes the unreachable orders from the specified OrderMap
     * @param src Source village
     * @param orders OrderMap to be filtered
     * @return OrderMap with unreachable orders
     */
    Delivery::OrderMap FilterUnreachable(uint src, Delivery::OrderMap& orders);

    static Menu* _menu; ///< Menu associated with the HydrographicNetwork class
};

#endif // HYDOGRAPHIC_NETWORK_H_
