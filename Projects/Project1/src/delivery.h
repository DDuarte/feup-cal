#ifndef DELIVERY_H_
#define DELIVERY_H_

#include "utils.h"

#include <vector>
#include <unordered_map>

class Order;
class ByteBuffer;
class Menu;
class HydrographicNetwork;

/**
 * Encapsulates information of a delivery from a single source point to
 *  multiple destinations
 */
class Delivery
{
public:
    typedef std::unordered_map<uint, std::vector<Order>> OrderMap;
    typedef std::unordered_map<uint, std::pair<uint, uint>> BoatMap;

    /// Constructor
    Delivery(uint sourceVillage, double boatCapacity, double supportVesselCapacity, uint numberOfSupportVessels) :
        _sourceVillage(sourceVillage), _boatCapacity(boatCapacity), _supportVesselCapacity(supportVesselCapacity),
        _numberOfSupportVessels(numberOfSupportVessels) { }

    uint GetSourceVillage() const { return _sourceVillage; } ///< Gets source village id of all orders in this delivery
    void SetSourceVillage(uint sourceVillage) { _sourceVillage = sourceVillage; } ///< Sets source village id of all orders in this delivery

    double GetBoatCapacity() const { return _boatCapacity; } ///< Gets volume/capacity of each boat
    void SetBoatCapacity(double boatCapacity) { _boatCapacity = boatCapacity; } ///< Sets volume/capacity of each boat

    double GetSupportVesselCapacity() const { return _supportVesselCapacity; } ///< Gets volume/capacity of the vessel of each boat
    void SetSupportVesselCapacity(double supportVesselCapacity) { _supportVesselCapacity = supportVesselCapacity; } ///< Sets volume/capacity of the vessel of each boat

    uint GetNumberOfSupportVessels() const { return _numberOfSupportVessels; } ///< Gets number of boats with support vessels
    void SetNumberOfSupportVessels(uint numberOfSupportVessels) { _numberOfSupportVessels = numberOfSupportVessels; } ///< Sets number of boats with support vessels

    OrderMap GetOrders() const { return _orders; } ///< Gets the map from destination village id to a collection of Orders
    void AddOrder(uint destinationVillage, const Order& order) { _orders[destinationVillage].push_back(order); } ///< Adds an Order to this Delivery

    const std::string& GetName() const { return _name; }
    void SetName(const std::string& name) { _name = name; }

    bool Save(ByteBuffer& bb) const; ///< Saves Delivery data to a ByteBuffer

    static Menu* GetMenu() { return _menu; } ///< Returns the menu for the Delivery class

private:
    uint _sourceVillage; ///< Source village id of all orders in this delivery
    OrderMap _orders; ///< Map from destination village id to a collection of Orders
    double _boatCapacity; ///< Volume/capacity of each boat
    double _supportVesselCapacity; ///< Volume/capacity of the vessel of each boat
    uint _numberOfSupportVessels; ///< Number of boats with support vessels
    std::string _name; ///< File name of this delivery

    static Menu* _menu; ///< Menu associated with the Delivery class
};

struct DeliveryRoute
{
    struct PathInfo
    {
        uint villageId;
        double transportedWeight;
        bool followsFlow;
        bool isIgarape;
    };

    typedef std::unordered_map<uint, std::vector<DeliveryRoute::PathInfo>> PathInfoMap;

    DeliveryRoute(const PathInfoMap& path, const Delivery::BoatMap& numberOfBoats, const Delivery::OrderMap& unreachable) : Path(path), NumberOfBoats(numberOfBoats), Unreachable(unreachable) { } ///< Constructor

    PathInfoMap Path;
    Delivery::BoatMap NumberOfBoats;
    Delivery::OrderMap Unreachable;

    static DeliveryRoute Load(std::istream& source, HydrographicNetwork& hn); ///< Loads orders from file
};

#endif // DELIVERY_H_
