#include "delivery.h"
#include "hydrographic_network.h"
#include "menu.h"
#include "loader.h"

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
        return DeliveryRoute(std::unordered_map<uint, std::vector<PathInfo>>());
    }

    Delivery del(villageSource, 100.0, 0.0, 0);
    for (auto& or : orders)
        for (auto& e : or.second)
            del.AddOrder(or.first, e);

    return hn.GetDeliveryPath(del);
}

bool Delivery::Save(ByteBuffer& bb) const
{
    return false;
}
