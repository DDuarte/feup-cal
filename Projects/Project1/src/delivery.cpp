#include "delivery.h"
#include "hydographic_network.h"

Delivery Delivery::Load(std::istream& source, HydrographicNetwork& hn)
{
    uint villageSource;
    std::unordered_map<uint, std::vector<Order>> orders;

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
        return Delivery(std::unordered_map<uint, std::vector<PathInfo>>());
    }

    return hn.GetDeliveryPath(villageSource, orders, 100.0);
}
