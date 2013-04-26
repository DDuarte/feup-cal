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
    ss << _orders.size() << std::endl;

    for (auto& a : _orders)
        for (auto& b : a.second)
            ss << a.first << b.GetVolume() << b.GetWeight() << std::endl;

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

        delivery->AddOrder(id, Order(volume, weight));
    }

    return delivery;
}
