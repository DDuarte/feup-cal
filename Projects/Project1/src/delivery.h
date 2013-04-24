#ifndef DELIVERY_H_
#define DELIVERY_H_

#include "defs.h"

#include <vector>
#include <unordered_map>

class HydrographicNetwork;

struct Delivery
{
    struct PathInfo
    {
        uint villageId;
        double transportedWeight;
        bool followsFlow;
        bool isIgarape;
    };

    Delivery(const std::unordered_map<uint, std::vector<PathInfo>>& path) : Path(path) { }
    Delivery(const std::unordered_map<uint, std::vector<PathInfo>>&& path) : Path(path) { }

    std::unordered_map<uint, std::vector<Delivery::PathInfo>> Path;
    static Delivery Load(std::istream& source, HydrographicNetwork& hn);
};

#endif // DELIVERY_H_
