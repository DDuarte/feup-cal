#include "delivery.h"
#include "hydrographic_network.h"
#include "menu.h"
#include "loader.h"

#include <sstream>

Menu* Delivery::_menu = Loader<Menu>("deliveryMenu.txt").Load();

bool Delivery::Save(ByteBuffer& bb) const
{
    std::ostringstream ss;

    ss << GetName() << std::endl;
    ss << GetSourceVillage() << std::endl;
    ss << GetBoatCapacity() << std::endl;
    ss << GetSupportVesselCapacity() << std::endl;
    ss << GetNumberOfSupportVessels() << std::endl;

    int totalOrders = 0;
    for (auto& a : _orders)
        for (auto& b : a.second)
            totalOrders++;

    ss << totalOrders << std::endl;

    for (auto& a : _orders)
        for (auto& b : a.second)
            ss << a.first <<' ' <<  b.GetVolume() <<' ' <<  b.GetWeight() << std::endl;

    std::string str = ss.str();
    bb.WriteBuffer(str.c_str(), str.size());

    return true;
}

Delivery* Delivery::Load(ByteBuffer& bb)
{
    std::istringstream ss(bb);

    std::string name;
    uint sourceVillage;
    double boatCapacity, supportVesselCapacity;
    uint numberOfSupportVessels;
    uint orderCount;

    ss >> name;
    ss >> sourceVillage;
    ss >> boatCapacity;
    ss >> supportVesselCapacity;
    ss >> numberOfSupportVessels;
    ss >> orderCount;

    Delivery* delivery = new Delivery(sourceVillage, boatCapacity, supportVesselCapacity, numberOfSupportVessels);
    delivery->SetName(name);

    for (uint i = 0; i < orderCount; ++i)
    {
        uint id;
        uint volume, weight;

        ss >> id >> volume >> weight;

        delivery->AddOrder(id, Order(weight, volume));
    }

    return delivery;
}

bool DeliveryRoute::Save(ByteBuffer& bb) const
{
    std::ostringstream ss;

    ss << "Path:" << std::endl;
    if (Path.empty())
    {
        ss << "\tNone - no orders." << std::endl;
    }
    else
    {
        for (const std::pair<uint, std::vector<DeliveryRoute::PathInfo>>& a : Path)
        {
            ss << "\tVillage Destination: " << a.first << std::endl;
            for (const DeliveryRoute::PathInfo& pi : a.second)
            {
                ss << "\t\t[Village Id: "         << pi.villageId         << "], ";
                ss <<     "[Transported Weight: " << pi.transportedWeight << "], ";
                ss <<     "[Follows Flow: "       << std::boolalpha << pi.followsFlow  << "], ";
                ss <<     "[Is Igarape: "         << std::boolalpha << pi.isIgarape    << "]" << std::endl;
            }
        }
    }

    ss << "Boats:" << std::endl;
    if (NumberOfBoats.empty())
    {
        ss << "\tNone - no boats." << std::endl;
    }
    else
    {
        for (const std::pair<uint, std::pair<uint, uint>>& a : NumberOfBoats)
        {
            ss << "\t[Village Destination: " << a.first         << "], ";
            ss <<   "[Number of Boats: "     << a.second.first  << "], ";
            ss <<   "[Total Weight: "        << a.second.second << "]" << std::endl;
        }
    }

    ss << "Non delivered orders:" << std::endl;
    if (Unreachable.empty())
    {
        ss << "\tNone - all sent." << std::endl;
    }
    else
    {
        for (const std::pair<uint, std::vector<Order>>& a : Unreachable)
        {
            ss << "\tVillage Destination: " << a.first << std::endl;
            for (const Order& b : a.second)
            {
                ss << "\t\t[Volume: " << b.GetVolume() << "], ";
                ss <<     "[Weight: " << b.GetWeight() << "]" << std::endl;
            }
        }
    }

    std::string str = ss.str();
    bb.WriteBuffer(str.c_str(), str.size());

    return true;
}
