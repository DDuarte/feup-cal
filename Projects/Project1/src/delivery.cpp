#include "delivery.h"
#include "hydrographic_network.h"
#include "menu.h"
#include "loader.h"

#include <sstream>

Menu* Delivery::_menu = Loader<Menu>("deliveryMenu.txt").Load();

DeliveryRoute DeliveryRoute::Load(std::istream& source, HydrographicNetwork& hn)
{
    uint villageSource;
    Delivery::OrderMap orders;

    try
    {
        source >> villageSource;

        uint orderCount;
        source >> orderCount;

        for (uint j = 0; j < orderCount; ++j)
        {
            uint villageDest, weight, volume;
            source >> villageDest >> weight >> volume;
            orders[villageDest].push_back(Order(weight, volume));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception """ << e.what() << """ occurred when loading Delivery." << std::endl;
        return DeliveryRoute(PathInfoMap(), Delivery::BoatMap(), Delivery::OrderMap());
    }

    Delivery del(villageSource, 100.0, 0.0, 0);
    for (auto& or : orders)
        for (auto& e : or.second)
            del.AddOrder(or.first, e);

    return hn.GetDeliveryPath(del);
}

bool Delivery::Save(ByteBuffer& bb) const
{
    std::ostringstream ss;

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
